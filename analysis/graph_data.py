import matplotlib.pyplot as plt
import numpy as np
# filename = '/Users/bmleedy/Desktop/kitchen_sensor_december/L-31.CSV'
# filename = '/Users/bmleedy/Desktop/kitchen_sensor_december/L-33.CSV'
# filename = '/Users/bmleedy/Desktop/kitchen_sensor_december/L-34.CSV'
filename = '/Users/bmleedy/Desktop/kitchen_sensor_december/L-35.CSV'
new_filename = filename + "_stripped"

with open(filename,'r') as file,open(new_filename,'w') as target:
    target.write(file.read().replace(',,\n','\n'))

data= np.loadtxt(new_filename, delimiter=',', unpack=True)

# # ID, writer time
# plt.plot(data[2,:],data[1,:], marker='.')
# plt.title('Data from the CSV File: TIME')
# plt.xlabel('ID')
# plt.ylabel('time')
# plt.show()

millis_to_hours = 1/1000/60/60

# # ID, average for sensor 0 (LPG)
THRESHOLD = 70
plt.plot(data[1, :]*millis_to_hours, data[0, :]/data[0, :] * THRESHOLD, 'r.')
plt.plot(data[1, :]*millis_to_hours, data[7, :], '.')
plt.title('Data from File: LPG')
plt.xlabel('ID')
plt.ylabel('LPG')
plt.show()
# Threshold at 70


# # ID, average for sensor 1 (CO)
THRESHOLD = 100
plt.plot(data[1, :]*millis_to_hours, data[0, :]/data[0, :] * THRESHOLD, 'r.')
plt.plot(data[1, :]*millis_to_hours, data[10,:], marker='.')
plt.title('Data from File: CO')
plt.xlabel('ID')
plt.ylabel('CO')
plt.show()
# Threshold at 100


# # ID, average for sensor 2 (O3)
THRESHOLD = 3.5
plt.plot(data[1, :]*millis_to_hours, data[0, :]/data[0, :] * THRESHOLD, 'r.')
plt.plot(data[1, :]*millis_to_hours, data[13, :], marker='.')
plt.title('Data from File: Ozone')
plt.xlabel('ID')
plt.ylabel('O3')
plt.show()
# Threshold at 3.5

# # ID, average for sensor 3 (Gas Leak)
THRESHOLD = 40
plt.plot(data[1, :]*millis_to_hours, data[0, :]/data[0, :] * THRESHOLD, 'r.')
plt.plot(data[1, :]*millis_to_hours, data[16, :], marker='.')
plt.title('Data from File: Gas Leak')
plt.xlabel('ID')
plt.ylabel('Gas Leak')
plt.show()
# Threshold at 40


# # ID, average for sensor 4 (Poison Gas)
THRESHOLD = 140
plt.plot(data[1, :]*millis_to_hours, data[0, :]/data[0, :] * THRESHOLD, 'r.')
plt.plot(data[1,:]*millis_to_hours, data[19,:], marker='.')
plt.title('Data from File: Poison Gas')
plt.xlabel('ID')
plt.ylabel('Poison Gas')
plt.show()
# Threshold at 140

# ID, average for sensor 5 (PM2.5)
THRESHOLD = 130
plt.plot(data[1, :]*millis_to_hours, data[0, :]/data[0, :] * THRESHOLD, 'r.')
plt.plot(data[1,:]*millis_to_hours,data[22,:], marker='.')
plt.title('Data from File: PM 2.5')
plt.xlabel('ID')
plt.ylabel('PM2.5')
plt.show()
# Threshold at 150
