**Puzzle solving pipeline**

1. Data Extraction
The ExtractionPipelines are initialized with pair of images (front and back scans of the pieces)
and produces the set of Pieces.
 - BinaryObjectExtractor extracts shapes from the image of back sides of the pieces
 - ObjectDetector determines approximate positions of the pieces on the image of their front sides
 - ShapeAlignOptimizer finds the exact matching of the extracted shapes of the pieces to pieces on the front image
 - PieceExtractor cuts the pieces along this shapes and creates the instances of the Piece class

2. Puzzle sloving
The Solver computes the combinatoric solution for the set of Pieces:
 - CompatibilityTable stores scores for every pair of edges
 - FrameSolver computes the position of the frame pieces
 - InteriorSolver fills the interior of the puzzle

3. Visualization
The Visualizator produces for given combinatoric solution the image with solved puzzle
 - The GeometricLayoutComputer computes the exact postiition of the piece in the resulting image
