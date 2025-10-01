
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