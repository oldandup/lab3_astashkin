#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include "structs.h"
#include "logger.h"
#include <vector>
#include <sstream>
#include <iomanip>
#include <functional>

using namespace std;

template<typename T>
class GenericSearchEngine {
protected:
    Logger& logger;

public:
    GenericSearchEngine(Logger& log) : logger(log) {}

    virtual ~GenericSearchEngine() = default;

    vector<T> SearchById(const vector<T>& items, int id) {
        vector<T> results;
        for (const auto& item : items) {
            if (item.id == id) {
                results.push_back(item);
                break;
            }
        }
        logger.Log("SEARCH BY ID - ID: " + to_string(id) + (results.empty() ? " - No results" : " - Found"));
        return results;
    }

    vector<T> SearchByCondition(const vector<T>& items, function<bool(const T&)> condition, const string& description) {
        vector<T> results;
        for (const auto& item : items) {
            if (condition(item)) {
                results.push_back(item);
            }
        }
        stringstream ss;
        ss << description << " - Found: " << results.size();
        logger.Log(ss.str());
        return results;
    }
};

class SearchEngine : public GenericSearchEngine<Pipe>, public GenericSearchEngine<Compress> {
public:
    SearchEngine(Logger& log) : GenericSearchEngine<Pipe>(log), GenericSearchEngine<Compress>(log) {}

    vector<Pipe> SearchPipesById(const vector<Pipe>& pipes, int id) {
        return GenericSearchEngine<Pipe>::SearchById(pipes, id);
    }

    vector<Pipe> SearchPipesByKmMark(const vector<Pipe>& pipes, const string& kmMark) {
        return GenericSearchEngine<Pipe>::SearchByCondition(pipes,
            [&kmMark](const Pipe& p) { return p.km_mark.find(kmMark) != string::npos; },
            "SEARCH PIPE BY KM MARK - Query: '" + kmMark + "'");
    }

    vector<Pipe> SearchPipesByDiameter(const vector<Pipe>& pipes, int diameter) {
        return GenericSearchEngine<Pipe>::SearchByCondition(pipes,
            [diameter](const Pipe& p) { return p.diametr == diameter; },
            "SEARCH PIPE BY DIAMETER - Diameter: " + to_string(diameter) + " mm");
    }

    vector<Pipe> SearchPipesByRepair(const vector<Pipe>& pipes, bool repair) {
        return GenericSearchEngine<Pipe>::SearchByCondition(pipes,
            [repair](const Pipe& p) { return p.repair == repair; },
            "SEARCH PIPE BY REPAIR STATUS - Status: " + string(repair ? "On repair" : "Not on repair"));
    }

    vector<Pipe> SearchPipesByLength(const vector<Pipe>& pipes, double minLength, double maxLength) {
        return GenericSearchEngine<Pipe>::SearchByCondition(pipes,
            [minLength, maxLength](const Pipe& p) { return p.length >= minLength && p.length <= maxLength; },
            "SEARCH PIPE BY LENGTH - Range: " + to_string(minLength) + "-" + to_string(maxLength) + " km");
    }

    vector<Compress> SearchCompressById(const vector<Compress>& stations, int id) {
        return GenericSearchEngine<Compress>::SearchById(stations, id);
    }

    vector<Compress> SearchCompressByName(const vector<Compress>& stations, const string& name) {
        return GenericSearchEngine<Compress>::SearchByCondition(stations,
            [&name](const Compress& c) { return c.name.find(name) != string::npos; },
            "SEARCH CS BY NAME - Query: '" + name + "'");
    }

    vector<Compress> SearchCompressByClassification(const vector<Compress>& stations, const string& classification) {
        return GenericSearchEngine<Compress>::SearchByCondition(stations,
            [&classification](const Compress& c) { return c.classification.find(classification) != string::npos; },
            "SEARCH CS BY CLASSIFICATION - Query: '" + classification + "'");
    }

    vector<Compress> SearchCompressByStatus(const vector<Compress>& stations, bool working) {
        return GenericSearchEngine<Compress>::SearchByCondition(stations,
            [working](const Compress& c) { return c.working == working; },
            "SEARCH CS BY STATUS - Status: " + string(working ? "Working" : "Not working"));
    }

    vector<Compress> SearchCompressByWorkshopPercentage(const vector<Compress>& stations, double minPercent, double maxPercent) {
        return GenericSearchEngine<Compress>::SearchByCondition(stations,
            [minPercent, maxPercent](const Compress& c) { 
                if (c.workshop_count > 0) {
                    double percentage = (double)c.workshop_working / c.workshop_count * 100;
                    return percentage >= minPercent && percentage <= maxPercent;
                }
                return false;
            },
            "SEARCH CS BY WORKSHOP PERCENTAGE - Range: " + to_string(minPercent) + "%-" + to_string(maxPercent) + "%");
    }

    vector<Compress> SearchCompressByWorkshopCount(const vector<Compress>& stations, int minCount, int maxCount) {
        return GenericSearchEngine<Compress>::SearchByCondition(stations,
            [minCount, maxCount](const Compress& c) { return c.workshop_working >= minCount && c.workshop_working <= maxCount; },
            "SEARCH CS BY WORKING WORKSHOPS - Range: " + to_string(minCount) + "-" + to_string(maxCount));
    }
};

#endif
