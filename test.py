import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def inertia_tensor_for_rectangular_box(m, a, b, c):
    """
    Calculate the inertia tensor for a rectangular box (cuboid).
    
    Parameters:
    - m: mass of the cuboid (kg)
    - a: length of the cuboid along the x-axis (m)
    - b: width of the cuboid along the y-axis (m)
    - c: height of the cuboid along the z-axis (m)
    
    Returns:
    - inertia_tensor: the inertia tensor in the local coordinate frame (3x3 matrix)
    """
    I_xx = (1/12) * m * (b**2 + c**2)
    I_yy = (1/12) * m * (a**2 + c**2)
    I_zz = (1/12) * m * (a**2 + b**2)
    
    # The off-diagonal terms are zero
    inertia_tensor = np.array([[I_xx, 0, 0],
                               [0, I_yy, 0],
                               [0, 0, I_zz]])
    
    return inertia_tensor

# Helper function to create a simple inertia tensor (for demonstration purposes)
def create_inertia_tensor(mass, r_x, r_y, r_z):
    I_xx = (1/12) * mass * (r_y**2 + r_z**2)
    I_yy = (1/12) * mass * (r_x**2 + r_z**2)
    I_zz = (1/12) * mass * (r_x**2 + r_y**2)
    return np.array([[I_xx, 0, 0],
                     [0, I_yy, 0],
                     [0, 0, I_zz]])

# Function to plot the inertia tensor (representation of body rotation)
def plot_inertia_tensor(inertia_tensor, title="Inertia Tensor Visualization"):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    
    # Generate a 3D grid of points (representing the body)
    u = np.linspace(0, 2 * np.pi, 100)
    v = np.linspace(0, np.pi, 100)
    x = np.outer(np.cos(u), np.sin(v))
    y = np.outer(np.sin(u), np.sin(v))
    z = np.outer(np.ones(np.shape(u)), np.cos(v))
    
    # Apply inertia tensor to stretch/shrink the sphere
    inertia_tensor_sqrt = np.linalg.cholesky(inertia_tensor)  # Cholesky decomposition for scaling
    x_new, y_new, z_new = inertia_tensor_sqrt @ np.vstack([x.ravel(), y.ravel(), z.ravel()])
    
    # Reshape the transformed coordinates
    x_new = x_new.reshape(x.shape)
    y_new = y_new.reshape(y.shape)
    z_new = z_new.reshape(z.shape)
    
    # Plot the transformed sphere
    ax.plot_surface(x_new, y_new, z_new, color='b', alpha=0.5)
    ax.set_title(title)
    plt.show()

# Simulate a simple body with an inertia tensor
mass = 10  # mass of the body in kg
r_x, r_y, r_z = 10, 2, 1 # dimensions of the body (for simplicity)

# Local inertia tensor (in body space)
inertia_tensor_local = create_inertia_tensor(mass, r_x, r_y, r_z)

inertia_tensor_local = np.array([[1, 0, 0],  # Much larger value along x-axis
                                  [1/2, 1, 0],   # Smaller value along y-axis
                                  [0, 0, 1]])  # Even smaller value along z-axis

inertia_tensor_local = inertia_tensor_for_rectangular_box(10,60,6,6)


# Define a simple rotation matrix (e.g., rotating by 45 degrees around the Z-axis)
theta = np.radians(45)
rotation_matrix = np.array([[np.cos(theta), -np.sin(theta), 0],
                            [np.sin(theta), np.cos(theta), 0],
                            [0, 0, 1]])

# World inertia tensor (this is the transformed tensor after applying rotation)
inertia_tensor_world = rotation_matrix @ inertia_tensor_local @ rotation_matrix.T

# Inverse of inertia tensors
inverse_inertia_tensor_local = np.linalg.inv(inertia_tensor_local)
#inverse_inertia_tensor_local=inertia_tensor_local
inverse_inertia_tensor_world = np.linalg.inv(inertia_tensor_world)

# Display inverse inertia tensors
print("Inverse Inertia Tensor (Local Space):")
print(inverse_inertia_tensor_local)

print("Inverse Inertia Tensor (World Space):")
print(inverse_inertia_tensor_world)

# Plot the body rotation effect based on the inertia tensor in local space
plot_inertia_tensor(inertia_tensor_local, title="Inertia Tensor in Local Space")

# Plot the body rotation effect based on the inertia tensor in world space
plot_inertia_tensor(inertia_tensor_world, title="Inertia Tensor in World Space")
