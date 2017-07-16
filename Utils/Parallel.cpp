/**
 * Basic toolkit for support of parallel execution
 */
namespace Parallel {
	
	using boost::thread;
	using boost::thread_group;

	// parallel execute the member function with signature f() on every given object
	// and gather the results
	template<class C>
	void ForEach(vector<C> &obj, void (C::*f)()) {
		for (unsigned int i = 0; i < obj.size();) {
			thread_group threads;
			for (int j = 0; i < obj.size() && j < NUM_THREADS; i++, j++) {
				cout << '.' << flush;
				threads.add_thread(new thread(boost::bind(f, &obj[i])));
			}
			threads.join_all();
		}
	}
	
	// parallel execute the member function with signature f(I param) on every given object
	template<class C, class I>
	void ForEach(vector<C> &obj, void (C::*f)(const I &param), const I &param) {
		for (unsigned int i = 0; i < obj.size();) {
			thread_group threads;
			for (int j = 0; i < obj.size() && j < NUM_THREADS; i++, j++) {
				cout << '.' << flush;
				threads.add_thread(new thread(boost::bind(f, &obj[i], param)));
			}
			threads.join_all();
		}
	}
	
}
