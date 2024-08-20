from matplotlib import pyplot as plt
import numpy as np
import json

with open("json_data.txt", "r") as file:
    filecontent = file.read()
    lines = filecontent.splitlines()

jx = json.loads(filecontent)

def smooth_this(new: int, old: int, percentage: int):
    percentage = percentage  / 100
    val = new * (percentage) + old * ( 1 - percentage)
    return val

def smooth_sensor(percentage=10):
    axl, ayl, azl = [], [], []
    for idx in range(len(jx)):
        curr = jx[idx]['accel']
        x, y, z = curr['x'], curr['y'], curr['z']
        if len(axl) > 0:
            x = smooth_this(x, axl[idx-1], percentage)
            y = smooth_this(y, ayl[idx-1], percentage)
            z = smooth_this(z, azl[idx-1], percentage)
        axl.append(x)
        ayl.append(y)
        azl.append(z)
    plt.title(f"percentage: {percentage}")
    plt.plot(axl)
    plt.plot(ayl)
    plt.plot(azl)
    plt.show()
    return (axl, ayl, azl)

data = smooth_sensor(10)
mapped = list(zip(data[0], data[1], data[2]))
points = np.array( mapped ).T
fig = plt.figure()
ax = fig.add_subplot(111, projection = '3d')
ax.plot(points[0], points[1], points[2], marker = 'x')
ax.scatter(*points.T[0], color = 'red')
plt.show()


import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np

points = np.array([[ 0.86063431,  0.07119279,  1.70377142],
                       [ 0.86391084,  0.07014899,  1.72184785],
                       [ 0.86332177,  0.069444  ,  1.71182579],
                       [ 0.86192988,  0.06913941,  1.69818289],
                       [ 0.86166436,  0.06916367,  1.69527615]]).T
    
fig = plt.figure()
ax = fig.add_subplot(111, projection = '3d')
ax.plot(points[0], points[1], points[2], marker = 'x')
ax.scatter(*points.T[0], color = 'red')
plt.show()



