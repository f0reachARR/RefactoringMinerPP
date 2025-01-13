package gr.uom.java.xmi.diff;

import gr.uom.java.xmi.LocationInfo;

import java.util.ArrayList;
import java.util.List;

public class RefactoringScope {
    private final List<LocationInfo> affectedLocations;

    public RefactoringScope() {
        this.affectedLocations = new ArrayList<>();
    }

    public RefactoringScope addAffectedLocation(LocationInfo loc) {
        affectedLocations.add(loc);
        return this;
    }

    public RefactoringScope addAffectedLocations(List<LocationInfo> locs) {
        affectedLocations.addAll(locs);
        return this;
    }

    public List<LocationInfo> getAffectedLocations() {
        return affectedLocations;
    }

    public int getAffectedLinesCount() {
        return affectedLocations.stream().mapToInt(t -> t.getEndLine() - t.getStartLine() + 1).sum();
    }
}
