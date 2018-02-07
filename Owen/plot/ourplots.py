import matplotlib.pyplot as plt
N=[1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000]

own_force = [0.803743, 3.272010, 7.558098, 13.164002, 20.807490, 29.210683, 35.383736, 46.758236, 58.576737, 71.704201]
master = [0.817004,3.262522,7.297768,13.342303,20.262850,29.221672,39.695441,52.099932,65.762871,81.030054]
plt.plot(N, master, 'ro')
plt.plot(N, own_force, 'bo')
plt.axis([0, 12000, 0, 100])
plt.legend(['Original', 'Improved'])
plt.xlabel('N - particles')
plt.ylabel('Time in seconds')
plt.title('Runtime plot')
plt.show()