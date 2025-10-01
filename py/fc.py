import socket
import json
import numpy as np
import time
import math

# ---------- hover calc ----------
m = 1.5               # kg
g = 9.81              # m/s^2
num_motors = 4
thrust_coefficient = 0.92  # N per unit rotor speed (set this to your sim's value)

total_weight = m * g
per_motor_N = total_weight / num_motors
motor_speed_hover = per_motor_N / thrust_coefficient

safety = 1.10   # 10% headroom
motor_speed_with_margin = motor_speed_hover * safety

print("total_weight (N):", total_weight)
print("per_motor (N):", per_motor_N)
print("motor_speed_hover:", motor_speed_hover)
print("motor_speed_with_margin:", motor_speed_with_margin)

# ---------- Quaternion helpers ----------
def quaternion_to_euler(q):
    x, y, z, w = q
    sinr_cosp = 2 * (w * x + y * z)
    cosr_cosp = 1 - 2 * (x * x + y * y)
    roll = math.atan2(sinr_cosp, cosr_cosp)

    sinp = 2 * (w * y - z * x)
    pitch = math.asin(sinp) if abs(sinp) < 1 else math.copysign(math.pi / 2, sinp)

    siny_cosp = 2 * (w * z + x * y)
    cosy_cosp = 1 - 2 * (y * y + z * z)
    yaw = math.atan2(siny_cosp, cosy_cosp)

    return roll, pitch, yaw

def quaternion_multiply(q1, q2):
    x1, y1, z1, w1 = q1
    x2, y2, z2, w2 = q2
    x = w1*x2 + x1*w2 + y1*z2 - z1*y2
    y = w1*y2 - x1*z2 + y1*w2 + z1*x2
    z = w1*z2 + x1*y2 - y1*x2 + z1*w2
    w = w1*w2 - x1*x2 - y1*y2 - z1*z2
    return [x, y, z, w]

def rotate_quaternion_for_opengl(q):
    angle_rad = math.pi  # 180 degrees
    sin_half = math.sin(angle_rad / 2)
    cos_half = math.cos(angle_rad / 2)
    rotation_q = [sin_half, 0, 0, cos_half]
    return quaternion_multiply(rotation_q, q)

# ---------- Improved PID (derivative-on-measurement, filtering + anti-windup) ----------
class PID:
    def __init__(self, kp, ki=0.0, kd=0.0,
                 out_min=None, out_max=None,
                 i_min=None, i_max=None,
                 deriv_filter_tau=0.02):
        self.kp = float(kp)
        self.ki = float(ki)
        self.kd = float(kd)
        self.integral = 0.0
        self.last_meas = None           # used for derivative-on-measurement
        self.d_filtered = 0.0
        self.tau = float(deriv_filter_tau)
        self.out_min = out_min
        self.out_max = out_max
        self.i_min = i_min
        self.i_max = i_max

    def update(self, setpoint, dt, meas =0.0, deadband=0.0):
        # safety
        if dt <= 0.0:
            return 0.0

        # error w/ deadband
        error = setpoint - meas
        
        if abs(error) < deadband:
            error = 0.0

        # integral with clamps (anti-windup)
        self.integral += error * dt
        if self.i_min is not None:
            self.integral = max(self.i_min, self.integral)
        if self.i_max is not None:
            self.integral = min(self.i_max, self.integral)

        # derivative on measurement (less noise)
        if self.last_meas is None:
            d_meas = 0.0
        else:
            d_meas = (meas - self.last_meas) / dt   # note: derivative of measurement

        # first-order low-pass filter for derivative
        alpha = dt / (self.tau + dt)
        self.d_filtered = (1 - alpha) * self.d_filtered + alpha * d_meas

        self.last_meas = meas

        # PID output: P + I - Kd * d(meas)/dt
        out = self.kp * error + self.ki * self.integral - self.kd * self.d_filtered

        # output clamp
        if self.out_min is not None:
            out = max(self.out_min, out)
        if self.out_max is not None:
            out = min(self.out_max, out)

        return out

# ---------- Mixer ----------
def mixer(thrust, pitch, roll, yaw):
    speeds = {
        "type": "set_speeds",
        "front_right":thrust + pitch - roll - yaw,
        "front_left":  thrust + pitch + roll + yaw,
        "back_left":   thrust - pitch + roll - yaw,
        "back_right":  thrust - pitch - roll + yaw
    }

    MAX_MOTOR = 26.5
    MIN_MOTOR = 0.0
    for key in ["front_right", "front_left", "back_left", "back_right"]:
        speeds[key] = max(MIN_MOTOR, min(MAX_MOTOR, speeds[key]))
    return speeds

# ---------- Networking ----------
UDP_IP = "127.0.0.1"
UDP_PORT = 12346
TARGET_IP = "127.0.0.1"
TARGET_PORT = 12345

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind((UDP_IP, UDP_PORT))
sock.settimeout(1.0)
send_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# ---------- Controllers (use improved PID API) ----------
# Suggested safe starting gains (you can tune later)
alt_pid = PID(kp=0.6, ki=0.002, kd=0.51,
              out_min=-3.0, out_max=3.0,
              i_min=-2.0, i_max=2.0,
              deriv_filter_tau=0.02)

# pos_pid_x = PID(kp=0.0065, ki=0.000, kd=0.003)


# pos_pid_z = PID(kp=0.0065, ki=0.000, kd=0.003)


# pos_pid_x = PID(kp=0.015, ki=0.0005, kd=0.01)


# pos_pid_z = PID(kp=0.015, ki=0.0005, kd=0.01)



pos_pid_x = PID(kp=0.5, ki=0.012, kd=0.81)


pos_pid_z = PID(kp=0.5, ki=0.012, kd=0.81)


# pos_pid_z = PID(kp=0.6, ki=0.0, kd=0.01,
#                 out_min=-math.radians(15), out_max=math.radians(15))

# pos_pid_z = PID(kp=0.00, ki=0.00, kd=0.00, 
#                 out_min=-math.radians(15), out_max=math.radians(15),
#                 i_min=-math.radians(10), i_max=math.radians(10))

att_pid_roll  = PID(kp=0.3315, ki=0.001, kd=0.3)
att_pid_pitch  = PID(kp=0.3315, ki=0.001, kd=0.3)
# att_pid_roll  = PID(kp=.125, ki=0.0, kd=0.00)
# att_pid_pitch  = PID(kp=0.125, ki=0.0, kd=0.00)
#att_pid_pitch = PID(kp=0.06, ki=0.0, kd=0.15)
yaw_pid       = PID(kp=.00, ki=0.00, kd=0.0000)

# If you want position loops later, create them with same pattern:
# pos_pid_x = PID(...); pos_pid_z = PID(...)

# ---------- Target / ramp ----------
desired_pos = np.array([15.0, 20.0, -25.0])  # target center altitude
target_pos = np.array([0, 0.5, 0.0])    # start
ramp_factor = 0.02                         # ramp per loop (smaller -> smoother)
current_yaw_target = 0.0

# ---------- Hover thrust from earlier calculation ----------
#hover_thrust = float(motor_speed_with_margin)   # motor units per motor for hover + margin
hover_thrust = 4.1   # motor units per motor for hover + margin
print("using hover_thrust:", hover_thrust)

time.sleep(2.5)

# ---------- Main loop ----------
last_time = time.time()
print("Listening for messages...")
initialized = False   # flag to init PID last_meas on first reading
flag = True
try:
    while True:
        try:
            data, _ = sock.recvfrom(1024)
            msg = json.loads(data.decode())

            if msg.get("type") != "sensor_fusion":
                continue

            now = time.time()
            dt = now - last_time
            last_time = now
            if dt <= 0.0:
                continue
            #print(dt)
            # --- Current state ---
            pos = np.array(msg["fused_position"])   # [x, y, z]
            raw_q = msg["fused_orientation"]
            q_opengl = rotate_quaternion_for_opengl(raw_q)
            roll, pitch, yaw = quaternion_to_euler(q_opengl)
            print(roll,pitch,yaw)

            if np.linalg.norm(pos-desired_pos) < 1.5  and flag != False:
                target_pos = pos
                desired_pos =  np.array([-15.0, 2.0, 5.0])
                flag = False

           # print(roll,yaw,pitch)

            # --- Initialize PID derivatives to avoid startup spike ---
            if not initialized:
                alt_pid.last_meas = pos[1]
                pos_pid_x.last_meas = pos[0]
                pos_pid_z.last_meas = pos[2]
                initialized = True

            # --- Ramp target position & yaw smoothly ---
            target_pos += (desired_pos - target_pos) * ramp_factor
            current_yaw_target += (0.0 - current_yaw_target) * ramp_factor

            # --- Position errors ---
            pos_error = target_pos - pos
            yaw_error = current_yaw_target - pitch

            # --- Altitude (Y) control ---
            thrust_correction = alt_pid.update(setpoint=target_pos[1],
                                            meas=pos[1],
                                            dt=dt,
                                            deadband=0.03)
               # ~3cm deadband
            thrust = hover_thrust + thrust_correction
            thrust = max(0.0, min(10.5, thrust))  # safety clamp

            # --- X/Z position → desired angles ---
            # In Y-up OpenGL:
            # X error → desired roll (roll right/left)
            # Z error → desired pitch (pitch forward/back)
           # print(target_pos[0]-pos[0])
            desired_roll = pos_pid_x.update(setpoint=target_pos[0], meas=pos[0], dt=dt)
            desired_pitch = pos_pid_z.update(setpoint=target_pos[2], meas=pos[2], dt=dt)
            #print(desired_pitch)
            #print("yyyyyyyyyyyy")
            # --- Attitude PID ---
           # print(pitch)
            max_angle = math.radians(10)  # Example: 20 degrees
            desired_roll = max(-max_angle, min(max_angle, desired_roll))
            desired_pitch = max(-max_angle, min(max_angle, desired_pitch))
            roll_cmd  = att_pid_roll.update(desired_roll, meas=roll, dt=dt)
            pitch_cmd = att_pid_pitch.update(-desired_pitch ,meas= yaw, dt=dt)
            yaw_cmd   = yaw_pid.update(yaw_error, dt=dt)

            #roll_cmd = min(.005, max(-.005, roll_cmd))  # safety clamp
            # pitch_cmd = min(.005, max(-.005, pitch_cmd))  # safety clamp


            # --- Motor mixing ---
            motor_speeds = mixer(thrust,pitch_cmd, roll_cmd, yaw_cmd)
            send_sock.sendto(json.dumps(motor_speeds).encode(), (TARGET_IP, TARGET_PORT))
            array = [roll,yaw,pitch]
            # --- Debug ---
            print(f"pos={pos}, target={target_pos}, thrust={thrust:.3f}, thrust_corr={thrust_correction:.3f}, "
                f"roll_cmd={roll_cmd:.6f}, pitch_cmd={pitch_cmd:.6f}, yaw_cmd={yaw_cmd:.6f}")
        except socket.timeout:
            continue

except KeyboardInterrupt:
    print("Shutting down.")
    sock.close()
