/**
 * Class handling the processing of the command line parameters
 * possble parameters are:
 * -f list of images of the pieces scanned from the front side
 * -b list of images of the pieces scanned from the back side in the same order
 *    as corresponding front images
 * -o name of output file
 */
class Settings {
	
	vector<string> frontImages, backImages;
	string outputFileName;
	
	public:
	
	Settings(int argc, char** argv) {
		outputFileName = "output.jpg";
		
		for (int i = 0; i < argc; i++) {
			string param = argv[i];
			if (param == "-f") {
				while (i+1 < argc && argv[i+1][0] != '-')
					frontImages.push_back(argv[++i]);
			}
			if (param == "-b") {
				while (i+1 < argc && argv[i+1][0] != '-')
					backImages.push_back(argv[++i]);
			}
			if (param == "-o") {
				outputFileName = argv[++i];
			}
		}
	}
	
	vector<string> getFrontFileNames() const {
		return frontImages;
	}
	
	vector<string> getBackFileNames() const {
		return backImages;
	}
	
	string getOutputFileName() const {
		return outputFileName;
	}
	
};
