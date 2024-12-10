#ifndef DATASET_HPP
#define DATASET_HPP

#include <string>
#include <vector>
#include <stdexcept>

   struct Pollutant {
    std::string id;                // Unique identifier (e.g., @id)
    std::string time;              // Time of the measurement
    std::string pollutantName;     // Name of the pollutant
    double concentration;          // Concentration value
    std::string complianceStatus;  // Compliance status (Compliant, Near Limit, Exceeds Limit)
    std::string location;          // Location of the sampling point
    
    Pollutant(const std::string& id, const std::string& time, const std::string& pollutantName,
              double concentration, const std::string& complianceStatus, const std::string& location)
        : id(id), time(time), pollutantName(pollutantName), concentration(concentration),
          complianceStatus(complianceStatus), location(location) {}
};



class PollutantDataset {
public:
    void loadData(const std::string& filename);
    std::vector<Pollutant> getData() const;

private:
    std::vector<Pollutant> data;
    void checkDataExists() const;
};

#endif
