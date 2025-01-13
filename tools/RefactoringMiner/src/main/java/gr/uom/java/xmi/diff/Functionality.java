package gr.uom.java.xmi.diff;

import gr.uom.java.xmi.LocationInfo;

public class Functionality {
    private final LocationInfo locationInfo;
    private final String functionalityType;

    public Functionality(LocationInfo locationInfo, String functionalityType) {
        this.locationInfo = locationInfo;
        this.functionalityType = functionalityType;
    }

    public LocationInfo getLocationInfo() {
        return locationInfo;
    }

    public String getFunctionalityType() {
        return functionalityType;
    }
}
