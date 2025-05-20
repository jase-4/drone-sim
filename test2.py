import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Function to compute the inertia tensor for a rectangular box
def inertia_tensor_for_rectangular_box(m, a, b, c):
    I_xx = (1/12) * m * (b**2 + c**2)
    I_yy = (1/12) * m * (a**2 + c**2)
    I_zz = (1/12) * m * (a**2 + b**2)
    return np.array([[I_xx, 0, 0],
                     [0, I_yy, 0],
                     [0, 0, I_zz]])

# Function to apply a rotation to the inertia tensor
def rotate_inertia_tensor(I_local, rotation_matrix):
    return rotation_matrix @ I_local @ rotation_matrix.T

# Plot function to visualize inertia tensor ellipsoid
def plot_inertia_tensor(I, title="Inertia Ellipsoid"):
    # Create a meshgrid for plotting
    theta = np.linspace(0, 2 * np.pi, 100)
    phi = np.linspace(0, np.pi, 100)
    x = np.outer(np.cos(theta), np.sin(phi))
    y = np.outer(np.sin(theta), np.sin(phi))
    z = np.outer(np.ones(np.size(theta)), np.cos(phi))

    # Stretch the sphere to match the inertia tensor
    scale_x = np.sqrt(I[0, 0])  # Inertia in x-axis
    scale_y = np.sqrt(I[1, 1])  # Inertia in y-axis
    scale_z = np.sqrt(I[2, 2])  # Inertia in z-axis

    # Create the 3D plot
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.plot_surface(x * scale_x, y * scale_y, z * scale_z, color='b', alpha=0.3)

    # Set axes to be independently scaled
    ax.set_box_aspect([scale_x, scale_y, scale_z])  # Aspect ratio based on moments of inertia

    # Labels and title
    ax.set_xlabel('X-axis')
    ax.set_ylabel('Y-axis')
    ax.set_zlabel('Z-axis')
    ax.set_title(title)

    plt.show()

# Define the rotation matrix for transforming to world space (e.g., 45 degrees rotation around the z-axis)
angle = np.radians(45)
rotation_matrix = np.array([
    [np.cos(angle), -np.sin(angle), 0],
    [np.sin(angle), np.cos(angle), 0],
    [0, 0, 1]
])

# Test with a rectangular box (dimensions m=10, a=60, b=6, c=6)
I_local = inertia_tensor_for_rectangular_box(1000, 6, 6, 6)

# Apply rotation to get the world inertia tensor
I_world = rotate_inertia_tensor(I_local, rotation_matrix)

# Plot the inertia tensor in local space
plot_inertia_tensor(I_local, title="Local Inertia Ellipsoid")

# Plot the inertia tensor in world space
plot_inertia_tensor(I_world, title="World Inertia Ellipsoid")
