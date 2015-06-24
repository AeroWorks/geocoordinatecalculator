#include "main.h"
#include "location.h"
#include "utils.h"

#include <c++utilities/application/argumentparser.h>
#include <c++utilities/application/failure.h>

#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;
using namespace ConversionUtilities;
using namespace ApplicationUtilities;

Angle::AngularMeasure inputAngularMeasure = Angle::AngularMeasure::Degree;
Angle::OutputForm outputFormForAngles = Angle::OutputForm::Degrees;
SystemForLocations inputSystemForLocations = SystemForLocations::LatitudeLongitude;
SystemForLocations outputSystemForLocations = SystemForLocations::LatitudeLongitude;

int main(int argc, char *argv[])
{   
    try {
        ArgumentParser argparser;

        Argument convert("convert", "c", "Converts the given coordinate or location to the specified output form.");
        convert.setRequiredValueCount(1);
        convert.appendValueName("coordinate/location");

        Argument distance("distance", "d", "Computes the approximate distance in meters between two locations.");
        distance.setRequiredValueCount(2);
        distance.appendValueName("location 1");
        distance.appendValueName("location 2");

        Argument trackLength("track-length", "t", "Computes the approximate length in meters of a track given by a file containing trackpoints separated by new lines.");
        trackLength.setRequiredValueCount(1);
        trackLength.appendValueName("path");
        Argument circle("circle", "", "If present the distance between the first and the last trackpoints will be added to the total track length.");
        trackLength.setSecondaryArguments({&circle});

        Argument bearing("bearing", "b", "Computes the approximate initial bearing East of true North when traveling along the shortest path between the given locations.");
        bearing.setRequiredValueCount(2);
        bearing.appendValueName("location 1");
        bearing.appendValueName("location 2");

        Argument fbearing("final-bearing", string(), "Computes the approximate final bearing East of true North when traveling along the shortest path between the given locations.");
        fbearing.setRequiredValueCount(2);
        fbearing.appendValueName("location 1");
        fbearing.appendValueName("location 2");

        Argument midpoint("midpoint", "m", "Computes the approximate midpoint between the given locations.");
        midpoint.setRequiredValueCount(2);
        midpoint.appendValueName("location 1");
        midpoint.appendValueName("location 2");

        Argument destination("destination", string(), "Calculates destination point given distance and bearing from start point.");
        destination.setRequiredValueCount(3);
        destination.appendValueName("start");
        destination.appendValueName("distance");
        destination.appendValueName("bearing");

        Argument gmapsLink("gmaps-link", "", "Generates a Google Maps link for all locations given by a file containing locations separated by new lines.");
        gmapsLink.setRequiredValueCount(1);
        gmapsLink.appendValueName("path");

        Argument inputAngularMeasureArg("input-angular-measure", "i", "Use this option to specify the angular measure you use to provide angles (degree or radian; default is degree).");
        inputAngularMeasureArg.setRequiredValueCount(1);
        inputAngularMeasureArg.appendValueName("angular measure");
        inputAngularMeasureArg.setCombinable(true);

        Argument outputFormForAnglesArg("output-angle-form", "o", "Use this option to specify the output form for angles (degrees, minutes, seconds or radians; default is degrees).");
        outputFormForAnglesArg.setRequiredValueCount(1);
        outputFormForAnglesArg.appendValueName("form");
        outputFormForAnglesArg.setCombinable(true);

        Argument inputSystemForLocationsArg("input-location-system", string(), "Use this option to specify the geographic system you use to provide locations (latitude&longitue or UTM-WGS84).");
        inputSystemForLocationsArg.setRequiredValueCount(1);
        inputSystemForLocationsArg.appendValueName("system");
        inputSystemForLocationsArg.setCombinable(true);

        Argument outputSystemForLocationsArg("output-location-system", string(), "Use this option to specify which geographic system is used to display locations (latitude&longitue or UTM-WGS84).");
        outputSystemForLocationsArg.setRequiredValueCount(1);
        outputSystemForLocationsArg.appendValueName("system");
        outputSystemForLocationsArg.setCombinable(true);

        HelpArgument help(argparser);

        Argument version("version", "v", "Shows the version of this application.");

        argparser.setMainArguments({&convert, &distance, &trackLength, &bearing, &fbearing, &midpoint, &destination, &gmapsLink, &help, &version, &inputAngularMeasureArg, &outputFormForAnglesArg, &inputSystemForLocationsArg, &outputSystemForLocationsArg});

        argparser.parseArgs(argc, argv);

        if(inputAngularMeasureArg.isPresent()) {
            if(inputAngularMeasureArg.value(0) == "radian") {
                inputAngularMeasure = Angle::AngularMeasure::Radian;
            } else if(inputAngularMeasureArg.value(0) == "degree") {
                inputAngularMeasure = Angle::AngularMeasure::Degree;
            } else {
                cout << "Invalid angular measure given, see --help." << endl;
                return 0;
            }
        }

        if(outputFormForAnglesArg.isPresent()) {
            if(outputFormForAnglesArg.value(0) == "degrees") {
                outputFormForAngles = Angle::OutputForm::Degrees;
            } else if(outputFormForAnglesArg.value(0) == "minutes") {
                outputFormForAngles = Angle::OutputForm::Minutes;
            } else if(outputFormForAnglesArg.value(0) == "seconds") {
                outputFormForAngles = Angle::OutputForm::Seconds;
            } else if(outputFormForAnglesArg.value(0) == "radians") {
                outputFormForAngles = Angle::OutputForm::Radians;
            } else {
                cout << "Invalid output form for angles given, see --help." << endl;
                return 0;
            }
        }

        if(inputSystemForLocationsArg.isPresent()) {
            if(inputSystemForLocationsArg.value(0) == "latitude&longitue") {
                inputSystemForLocations = SystemForLocations::LatitudeLongitude;
            } else if(inputSystemForLocationsArg.value(0) == "UTM-WGS84") {
                inputSystemForLocations = SystemForLocations::UTMWGS84;
            } else {
                cout << "Invalid geographic coordinate system given, see --help." << endl;
                return 0;
            }
        }

        if(outputSystemForLocationsArg.isPresent()) {
            if(outputSystemForLocationsArg.value(0) == "latitude&longitue") {
                outputSystemForLocations = SystemForLocations::LatitudeLongitude;
            } else if(outputSystemForLocationsArg.value(0) == "UTM-WGS84") {
                outputSystemForLocations = SystemForLocations::UTMWGS84;
            } else {
                cout << "Invalid geographic coordinate system given, see --help." << endl;
                return 0;
            }
        }

        try {
            if(help.isPresent()) {
                cout << endl;
                printAngleFormatInfo(cout);
            } else if(version.isPresent()) {
                cout << "1.0.0";
            } else if(convert.isPresent()) {
                printConversion(convert.value(0));
            } else if(distance.isPresent()) {
                printDistance(distance.value(0), distance.value(1));
            } else if(trackLength.isPresent()) {
                printTrackLength(trackLength.value(0), circle.isPresent());
            } else if(bearing.isPresent()) {
                printBearing(bearing.value(0), bearing.value(1));
            } else if(fbearing.isPresent()) {
                printFinalBearing(fbearing.value(0), fbearing.value(1));
            } else if(midpoint.isPresent()) {
                printMidpoint(midpoint.value(0), midpoint.value(1));
            } else if(destination.isPresent()) {
                printDestination(destination.value(0), destination.value(1), destination.value(2));
            } else if(gmapsLink.isPresent()) {
                printMapsLink(gmapsLink.value(0));
            } else {
                cout << "No arguments given. See --help for available commands.";
            }
        } catch(Failure &ex) {
            cout << "The provided locations/coordinates couldn't be parsed correctly. " << ex.what() << endl;
            cout << endl;
            printAngleFormatInfo(cout);
        }
    } catch(Failure &ex) {
        cout << "Unable to parse arguments. " << ex.what() << endl << "See --help for available commands.";
    }

    cout << endl;
    return 0;
}


Location locationFromString(const string &userInput)
{
    switch(inputSystemForLocations) {
    case SystemForLocations::UTMWGS84: {
        Location l;
        l.setValueByProvidedUtmWgs4Coordinates(userInput);
        return l;
    } default:
        return Location(userInput, inputAngularMeasure);
    }
}

vector<Location> locationsFromFile(const string &path)
{
    // prepare reading
    fstream file;
    file.open(path, ios_base::in);
    if(!file)
        throw ios_base::failure("Unable to open the file \"" + path + "\".");
    file.exceptions(ios_base::badbit);
    string line;
    vector<Location> locations;
    while(getline(file, line)) {
        if(line.empty() || line.at(0) == '#')
            continue; // skip empty lines and comments
        locations.push_back(locationFromString(line));
    }
    return locations;
}

void printAngleFormatInfo(ostream &os)
{
    os << "To provide a location/trackpoint, use the following form:\n";
    os << "latitude,longitude\n";

    os << "\nUse one of the following forms to specify angles, if you use --input-angle-measure degree:\n";
    os << "[+-]DDD.DDDDD\n";
    os << "[+-]DDD:MM.MMMMM\n";
    os << "[+-]DDD:MM:SS.SSSSS\n";
    os << "D indicates degrees, M indicates minutes of arc, and S indicates seconds of arc (1 minute = 1/60th of a degree, 1 second = 1/3600th of a degree).\n";

    os << "You can use the following form where R indicates radians, if you use --input-angle-measure radian:\n";
    os << "[+-]RRR.RRRRR";
}

void printConversion(const string &coordinates)
{
    if(coordinates.find(',') == string::npos && coordinates.find('N') == string::npos && coordinates.find('E') == string::npos)
        cout << Angle(coordinates, inputAngularMeasure).toString(outputFormForAngles);
    else
        printLocation(locationFromString(coordinates));
}

void printDistance(const std::string &locationstr1, const std::string &locationstr2)
{
    printDistance(locationFromString(locationstr1)
                  .distanceTo(locationFromString(locationstr2)));
}

void printDistance(double distance)
{
    if(distance > 1000)
        cout << (distance / 1000.0) << " km";
    else
        cout << distance << " m";
}

void printTrackLength(const string &filePath, bool circle)
{
    try {
        vector<Location> locations(locationsFromFile(filePath));
        printDistance(Location::trackLength(locations, circle));
        cout << " (" << locations.size() << " trackpoints)";
    } catch(ios_base::failure &ex) {
        cout << "An IO failure occured when reading file from provided path: " << ex.what() << endl;
    }
}

void printBearing(const string &locationstr1, const string &locationstr2)
{
    cout << locationFromString(locationstr1).initialBearingTo(
                locationFromString(locationstr2))
            .toString(outputFormForAngles) << endl;
}

void printFinalBearing(const string &locationstr1, const string &locationstr2)
{
    cout << locationFromString(locationstr1).finalBearingTo(
                locationFromString(locationstr2))
            .toString(outputFormForAngles) << endl;
}

void printMidpoint(const string &locationstr1, const string &locationstr2)
{
    printLocation(Location::midpoint(
                      locationFromString(locationstr1),
                      locationFromString(locationstr2)));
}

void printDestination(const string &locationstr, const string &distancestr, const string &bearingstr)
{
    Location start = locationFromString(locationstr);
    double distance = numberFromString<double>(distancestr);
    Angle bearing(bearingstr, inputAngularMeasure);
    printLocation(start.destination(distance, bearing));
}

void printLocation(const Location &location)
{
    switch(outputSystemForLocations) {
    case SystemForLocations::LatitudeLongitude:
        cout << location.toString(outputFormForAngles);
        break;
    case SystemForLocations::UTMWGS84:
        cout << location.toUtmWgs4String();
        break;
    }
}


void printMapsLink(const string &filePath)
{
    try {
        vector<Location> locations(locationsFromFile(filePath));
        if(locations.size() > 0) {
            cout << "https://maps.google.de/maps?saddr=";
            const Angle::OutputForm outputForm = Angle::OutputForm::Degrees;
            cout << locations.front().toString(outputForm);
            if(locations.size() > 1) {
                cout << "&daddr=";
                cout << locations.at(1).toString(outputForm);
            }
            if(locations.size() > 2) {
                for(vector<Location>::const_iterator i = locations.cbegin() + 2, end = locations.cend(); i != end; ++i) {
                    cout << "+to:";
                    cout << i->toString(outputForm);
                }
            }
            cout << "&mra=mi&mrsp=2&sz=16&z=16";
        } else {
            throw Failure("At least one location is required to generate a link.");
        }
    } catch(ios_base::failure &ex) {
        cout << "An IO failure occured when reading file from provided path: " << ex.what() << endl;
    }
}
