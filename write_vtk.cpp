#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdint>
//#include <algorithm> // Include this header for std::reverse

// Function to read elements from file
std::vector<std::vector<int>> readElements(const std::string& filename) {
  std::ifstream file(filename);
  std::vector<std::vector<int>> elements;

  if (file.is_open()) {
    std::vector<int> row;
    int value;

    while (file >> value) {
      row.push_back(value);
      if (row.size() == 4) {  // Assuming each line has 4 values
	elements.push_back(row);
	row.clear();
      }
    }

    file.close();
  } else {
    std::cerr << "Unable to open file: " << filename << std::endl;
  }

  return elements;
}

// Function to read nodes from file
std::vector<std::vector<double>> readNodes(const std::string& filename) {
  std::ifstream file(filename);
  std::vector<std::vector<double>> nodes;

  if (file.is_open()) {
    std::vector<double> coords(2);

    while (file >> coords[0] >> coords[1]) {
      nodes.push_back(coords);
    }

    file.close();
  } else {
    std::cerr << "Unable to open file: " << filename << std::endl;
  }

  return nodes;
}

// Function to read the ith line from time series data file
std::vector<double> readTimeSeriesData(const std::string& filename, int lineIndex) {
  std::ifstream file(filename);
  std::vector<double> result;

  if (file.is_open()) {
    std::string line;
    int currentIndex = 0;

    while (std::getline(file, line)) {
      if (currentIndex == lineIndex) {
	std::istringstream iss(line);
	double value;
	while (iss >> value) {
	  result.push_back(value);
	}
	break;
      }
      currentIndex++;
    }

    file.close();
  } else {
    std::cerr << "Unable to open file: " << filename << std::endl;
  }

  return result;
}

bool isSystemLittleEndian() {
    uint16_t number = 0x1;
    return *(reinterpret_cast<uint8_t*>(&number)) == 0x1;
}

template<typename T>
void writeBinary(std::ofstream& vtkfile, const T& value) {
    T val = value;
   //if (isSystemLittleEndian()) {
   //    char* p = reinterpret_cast<char*>(&val);
   //    std::reverse(p, p + sizeof(T));
   //}
    vtkfile.write(reinterpret_cast<const char*>(&val), sizeof(T));
}


// Function to write point data to VTK file in binary format
void writeVTKUnstructuredBinary(const std::string& filename, const std::vector<std::vector<double>>& nodes,
				const std::vector<std::vector<int>>& elements,
				const std::vector<double>& u,
				const std::vector<double>& d) {
  std::ofstream vtkfile(filename, std::ios::out | std::ios::binary);

  if (vtkfile.is_open()) {
    vtkfile << "# vtk DataFile Version 3.0\n";
    vtkfile << "VTK from C++\n";
    vtkfile << "BINARY\n";
    vtkfile << "DATASET UNSTRUCTURED_GRID\n";

    // Write out the points
    vtkfile << "POINTS " << nodes.size() << " double\n";
    for (const auto& node : nodes) {
      writeBinary(vtkfile, node[0]);
      writeBinary(vtkfile, node[1]);
      writeBinary(vtkfile, 0.0);  // Assuming 2D points with z = 0.0
    }

    // Write out the cells (elements)
    int totalCellSize = elements.size() * (elements[1].size() + 1);
    vtkfile << "CELLS " << elements.size() << " " << totalCellSize << "\n";
    for (const auto& elem : elements) {
      int size = elem.size();
      writeBinary(vtkfile, size);
      for (int i = 0; i < elem.size(); i++) {
	int index = elem[i] - 1;
	writeBinary(vtkfile, index);
      }
    }

    // Write out cell types (VTK_QUAD for 2D quads, adjust as needed)
    vtkfile << "CELL_TYPES " << elements.size() << "\n";
    for (size_t i = 0; i < elements.size(); ++i) {
      int cellType = 9;  // VTK_QUAD is type 9 for 2D quads
      writeBinary(vtkfile, cellType);
    }

    // Write out point data
    vtkfile << "POINT_DATA " << nodes.size() << "\n";

    vtkfile << "VECTORS displacement double\n";
    std::vector<double> uu;
    for (int i = 0; i < u.size(); i = i + 2) {
      double ux = u[i];
      double uy = u[i + 1];
      double uz = 0.0;
      writeBinary(vtkfile, ux);
      writeBinary(vtkfile, uy);
      writeBinary(vtkfile, uz);
    }

    vtkfile << "SCALARS damage double\n";
    vtkfile << "LOOKUP_TABLE default\n";
    for (const auto& value : d) {
      writeBinary(vtkfile, value);
    }
    vtkfile.close();
  } else {
    std::cerr << "Unable to open file: " << filename << std::endl;
  }
}

// Function to write data to VTK file
void writeVTKUnstructured(const std::string& filename, const std::vector<std::vector<double>>& nodes,
			  const std::vector<std::vector<int>>& elements,
			  const std::vector<double>& u,
			  const std::vector<double>& d) {
  std::ofstream vtkfile(filename);

  if (vtkfile.is_open()) {
    vtkfile << "# vtk DataFile Version 3.0\n";
    vtkfile << "VTK from C++\n";
    vtkfile << "ASCII\n";
    vtkfile << "DATASET UNSTRUCTURED_GRID\n";

    // Write out the points
    vtkfile << "POINTS " << nodes.size() << " double\n";
    for (const auto& node : nodes) {
      vtkfile << node[0] << " " << node[1] << " 0.0\n";  // Assuming 2D nodes with z = 0.0
    }

    // Write out the cells (elements)
    int totalCellSize = 0;
    for (const auto& elem : elements) {
      totalCellSize += elem.size() + 1;  // +1 for number of points in this cell
    }

    vtkfile << "CELLS " << elements.size() << " " << totalCellSize << "\n";
    for (const auto& elem : elements) {
      vtkfile << elem.size() << " ";  // Number of points in this cell
      for (int i = 0; i < elem.size()-1; ++i) {
	vtkfile << elem[i] - 1 << " ";
      }
      vtkfile << elem.back() - 1 << "\n";
    }

    // Write out cell types (VTK_QUAD for 2D quads, adjust as needed)
    vtkfile << "CELL_TYPES " << elements.size() << "\n";
    for (size_t i = 0; i < elements.size(); ++i) {
      vtkfile << "9 ";  // VTK_QUAD is type 9 for 2D quads
    }
    vtkfile << "\n";

    // Write out point data
    vtkfile << "POINT_DATA " << nodes.size() << "\n";

    vtkfile << "VECTORS displacement double\n";
    for (int i = 0; i < u.size()-1; i = i + 2) {
      vtkfile << u[i] << " " << u[i+1] << " 0.0\n";
    }

    vtkfile << "SCALARS damage double\n";
    vtkfile << "LOOKUP_TABLE default\n";
    for (const auto& value : d) {
      vtkfile << value << " ";
    }
    vtkfile.close();
  } else {
    std::cerr << "Unable to open file: " << filename << std::endl;
  }
}


int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " <output_vtk_file> <time_series_displacement_data> <time_series_damage_data> <line_index>\n";
        return 1;
    }

    std::string elementsFile = "elements";
    std::string nodesFile = "nodes";
    std::string vtkFile = argv[1];
    std::string displacementFile = argv[2];
    std::string damageFile = argv[3];
    std::string x = argv[4];
    vtkFile += "_";
    for (int i = 0; i < 4 - x.size(); i++) {
      vtkFile += "0";
    }
    vtkFile += x + ".vtk";
    int lineIndex = std::stoi(x);


    // Read elements and nodes
    std::vector<std::vector<int>> elements = readElements(elementsFile);
    std::vector<std::vector<double>> nodes = readNodes(nodesFile);

    // Read the ith line from time series data file
    std::vector<double> u = readTimeSeriesData(displacementFile, lineIndex);
    std::vector<double> d = readTimeSeriesData(damageFile, lineIndex);

    // Write to VTK file
    writeVTKUnstructured(vtkFile, nodes, elements, u, d);
    //writeVTKUnstructuredBinary(vtkFile, nodes, elements, u, d);
    std::cout << "VTK file '" << vtkFile << "' written successfully.\n";

    return 0;
}
