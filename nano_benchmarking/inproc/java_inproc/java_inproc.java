import java.util.LinkedList;

public class java_inproc {
	
	public LinkedList<Integer> list;
	
	public java_inproc() {
		list = new LinkedList<>();
	}

	/**
	 * Main entry point.
	 * @param args thread count, update count, update size
	 */
	public static void main(String[] args) {
		try {
			// Prepare shared list
			java_inproc inproc = new java_inproc();
			
			int thread_count = Integer.parseInt(args[0]);
			int update_count = Integer.parseInt(args[1]);
			int update_size = Integer.parseInt(args[2]); // TODO
			
			// Start threads and execute update depending on parameters
			Runnable torun =new Runnable() {	
				@Override
				public void run() {
					// Number of update per thread
					for (int i = 0; i < update_count; i++) {
						synchronized(inproc.list) {
							inproc.list.add(i); // TODO : Size of update
						}
					}
				}
			};
			
			// Number of threads
			long startTime = System.nanoTime();
			Thread[] threads = new Thread[thread_count];
			for (int i = 0; i < thread_count; i++) {
				threads[i] = new Thread(torun);
			}
			
			//long startTime = System.nanoTime();
			for (int i = 0; i < thread_count; i++) {
				threads[i].start();
			}
	
			for (int i = 0; i < thread_count; i++) {
				threads[i].join();
			}
			long endTime = System.nanoTime();
			
			// Final timing output
			System.out.println((endTime - startTime)/1000000.0);
			
		} catch (Exception ex) {
			System.err.println("Error while processing : " + ex.getMessage());
		}
	}

}
