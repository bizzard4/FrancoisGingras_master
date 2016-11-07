import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt
import pandas
import sys


# Usage
# plot.py ddmmyyyy inproc|ipc|tcp uc|th|us
# plot.py ddmmyyyy all

# Print script usage
def printUsage():
	print("Usage : plot.py ddmmyyyy inproc|ipc|tcp uc|th|us")
	print("Usage : plot.py ddmmyyyy all")
	return

# Add a TH line on a plot
def plotResult(file, sign, legend):
	data = pandas.read_csv(file, sep=';')
	data_list = data.test_time.tolist()
	plt.plot(data.test_param_value.tolist(), data.test_time.tolist(), sign, label=legend)
	return

tech_list = {"inproc" : ["nano_inproc", "pthread_inproc", "java_inproc"], "ipc" : ["nano_ipc", "dns_ipc"], "tcp" : ["nano_tcp", "socket_tcp"]}
xlabel_list = {"th" : "Thread count", "uc" : "Update count", "us" : "Update size"}
sign_list = [".r", ".b", ".g"] # At most, 3 sign on the plot in the same time

# Filter parameters
plot_order = []
if (len(sys.argv)==4): # 2 params
	tech = sys.argv[2]
	if((tech != "inproc") and (tech != "ipc") and (tech != "tcp")):
		print("Wrong tech : inproc, ipc, tcp")
		printUsage()
		sys.exit(-1)
	param_type = sys.argv[3]
	if((param_type != "uc") and (param_type != "th") and (param_type != "us")):
		print("Wrong param type : th, uc, us")
		printUsage()
		sys.exit(-1)
	plot_order.append((tech, param_type))
elif (len(sys.argv)==3): # All
	if (sys.argv[2] != "all"):
		print("Wrong params : all?")
		printUsage()
		sys.exit(-1)

	# Add all plot order
	for tech_val in tech_list:
		for label_val in xlabel_list:
			if (label_val != "us"): # us not implemented for now
				plot_order.append((tech_val, label_val))
else:
	printUsage()
	sys.exit(-1)

result_set = sys.argv[1]

# Plot the requested chart
print(plot_order)
for order in plot_order:
	to_plot = tech_list[order[0]]
	count = 0
	for val in to_plot:
		plotResult("results/" + result_set + "/" + val + "_" + order[1] + ".csv", sign_list[count], val.upper() + " " + order[1].upper())
		count += 1

	# Plot all
	plt.ylabel("Time (ms)")
	plt.xlabel(xlabel_list[order[1]])
	plt.legend(bbox_to_anchor=(0.05, 0.95), loc=2, borderaxespad=0.)
	plt.savefig("results/" + result_set + "/" + order[0] + "_" + order[1] + ".png")
	plt.gcf().clear()