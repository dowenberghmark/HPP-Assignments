import matplotlib.pyplot as plt
N=[1, 2,  4, 8, 16]

#org1 = [[16.44 ,9.25 ,5.64 ,4.39 ,3.72], [17.02, 9.39, 5.59, 4.40, 3.63], [15.49, 8.77, 5.02, 4.47, 2.95], [15.69, 8.72, 4.83, 3.85, 2.83]]
org1 = [[15.56 ,8.79 ,4.86 ,3.96 ,2.94], [15.81, 8.83, 6.22, 4.53, 4.15], [16.65, 17.19, 6.02, 2.88, 2.81], [15.43, 8.60, 4.91, 4.17, 3.11], [14.27, 7.50, 4.09, 3.21, 2.28], [14.54, 7.45, 4.10, 2.91, 2.31]]


types=['r-','b-', 'g-', 'c-', 'm-', 'y-',]
for i in range(0,6):
    tmp = org1[i][0]
    for j in range(0,5):
        org1[i][j] =   tmp / org1[i][j] 
plt.plot(N, N, '--')
for i in range(0,6):    
    plt.plot(N, org1[i], types[i])
#plt.plot(N, own_force, 'bo')
#plt.axis([0, 12000, 0, 100])
plt.legend(['Ideal','Parallel force', 'Task based','Recursive','Updating positions', 'Updating positions - with serial opt','All the for loops'])
plt.xlabel('N - Threads')
plt.ylabel('Speed up')
plt.title('Speed-up plot - 5000 stars 100 timesteps')
plt.show()

#org2 = [[5.35, 3.72, 2.62, 1.58, 1.51], [5.20, 3.14, 1.89, 1.62, 1.22], [4.78, 2.83, 1.75, 1.48, 1.41], [4.67, 2.83, 1.70, 1.39, 1.22]]
org2 = [[4.95, 2.67, 1.76, 1.34, 0.90], [4.74, 2.71, 2.03, 1.73, 1.45],[5.04, 5.33, 2.08, 1.31, 1.22], [4.63, 2.61, 1.56, 1.44, 1.15], [4.30, 2.51, 1.35, 1.23, 0.81], [4.31, 2.32, 1.68, 1.75, 1.00]]

for i in range(0,6):
    tmp = org2[i][0]
    for j in range(0,5):
        org2[i][j] =   tmp / org2[i][j] 

plt.plot(N, N, '--')
for i in range(0,6):    
    plt.plot(N, org2[i], types[i])
#plt.plot(N, own_force, 'bo')
#plt.axis([0, 12000, 0, 100])
#    plt.legend(['Ideal','Parallel force', 'Updating positions', 'Including timestep', 'Less synchronization'])
plt.legend(['Ideal','Parallel force', 'Task based','Recursive','Updating positions', 'Updating positions - with serial opt','All the for loops'])
plt.xlabel('N - Threads')
plt.ylabel('Speed up')
plt.title('Speed-up plot - 1000 stars 100 timesteps')
plt.show()
