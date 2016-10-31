import matplotlib.pyplot as plt
import pandas
import sys

# Usage
# plot.py ddmmyyyy inproc|ipc|tcp uc|th|us

# TODO
# - Function to save all (or partial)

# Print script usage
def printUsage():
	print("Usage : plot.py ddmmyyyy inproc|ipc|tcp uc|th|us")
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
if (len(sys.argv) < 4):
	printUsage()
	sys.exit(-1)
result_set = sys.argv[1]
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

# Plot the requested chart
to_plot = tech_list[tech]
count = 0
for val in to_plot:
	plotResult("results/" + result_set + "/" + val + "_" + param_type + ".csv", sign_list[count], tech.upper() + " " + param_type.upper())
	count += 1

# Plot all
plt.ylabel("Time (ms)")
plt.xlabel(xlabel_list[param_type])
plt.legend(bbox_to_anchor=(0.05, 0.95), loc=2, borderaxespad=0.)
plt.savefig("test.png")