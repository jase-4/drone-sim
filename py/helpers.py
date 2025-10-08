import math
import numpy as np

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
    angle_rad = math.pi  
    sin_half = math.sin(angle_rad / 2)
    cos_half = math.cos(angle_rad / 2)
    rotation_q = [sin_half, 0, 0, cos_half]
    return quaternion_multiply(rotation_q, q)


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
