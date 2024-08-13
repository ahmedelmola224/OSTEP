import matplotlib.pyplot as plt

# Define the x-values
system_ticks = [0 , 775 , 1779 , 4656 , 5012 , 5411]

# Define the y-values for each function
process_1_ticks = [0, 128 , 292 , 767 , 812  , 899]
process_2_ticks = [0, 245 , 552 , 1541, 1647 , 1772]
process_3_ticks = [0, 392 ,925  , 2338, 2543 , 2730]

# Create a new figure and axis
fig, ax = plt.subplots()

# Plot each function with a different color and shape
ax.plot(system_ticks, process_1_ticks, 'b-', label='process 1 with 10 tickets')
ax.plot(system_ticks, process_2_ticks, 'r-', label='process 2 with 20 tickets')
ax.plot(system_ticks, process_3_ticks, 'g-', label='process 3 with 30 tickets')

# Add a legend and axis labels
ax.legend()
ax.set_xlabel('system ticks')
ax.set_ylabel('process ticks')
ax.set_title('Lottery scheduling for three processes in XV6')
plt.savefig('plot.png')
# Show the plot
plt.show()
