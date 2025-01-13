package gr.uom.java.xmi.diff;

import gr.uom.java.xmi.LocationInfo;
import gr.uom.java.xmi.decomposition.AbstractCodeMapping;
import gr.uom.java.xmi.decomposition.AbstractExpression;
import gr.uom.java.xmi.decomposition.CompositeStatementObject;
import gr.uom.java.xmi.decomposition.UMLOperationBodyMapper;

import java.util.ArrayList;
import java.util.List;

public class FunctionalityInspector {
    private final UMLModelDiff modelDiff;
    private final List<Functionality> detectedFunctionalities;

    public FunctionalityInspector(UMLModelDiff modelDiff) {
        this.modelDiff = modelDiff;
        this.detectedFunctionalities = new ArrayList<>();
    }

    public void process() {
        addClasses();
        addGeneralizations();
        addRealizations();
        addAttributes();
        addOperations();
        addStatements();
    }

    private void addClasses() {
        modelDiff.getAddedClasses().forEach(t ->
                detectedFunctionalities.add(new Functionality(t.getLocationInfo(), "added class"))
        );
    }

    private void addGeneralizations() {
        modelDiff.getAddedGeneralizations().forEach(t -> {
           LocationInfo newLoc = t.getChild().getLocationInfo().copy();
           newLoc.reduceToFirstLine();

           detectedFunctionalities.add(new Functionality(newLoc, "added generalization"));
        });

        //TODO check generalizationDiffList if the changed generalization is new. i.e. the parent class was not just renamed etc.
    }

    private void addRealizations() {
        modelDiff.getAddedRealizations().forEach(t -> {
            LocationInfo newLoc = t.getClient().getLocationInfo().copy();
            newLoc.reduceToFirstLine();

            detectedFunctionalities.add(new Functionality(newLoc, "added realization"));
        });

        //TODO check realizationDiffList if the changed realization is new. i.e. the parent class was not just renamed etc.
    }

    private void addAttributes() {
        addAttributesFromClassDiffs(modelDiff.getCommonClassDiffList());
        addAttributesFromClassDiffs(modelDiff.getClassMoveDiffList());
        addAttributesFromClassDiffs(modelDiff.getClassRenameDiffList());

        //TODO check if classMergeDiffList and classSplitDiffList need to be added here
    }

    private void addAttributesFromClassDiffs(List<? extends UMLClassBaseDiff> classDiffs) {
        for (UMLClassBaseDiff classDiff : classDiffs) {
            classDiff.getAddedAttributes().forEach(t -> detectedFunctionalities.add(new Functionality(t.getLocationInfo(), "added attribute")));

            classDiff.getAttributeDiffList().stream()
                    .filter(t -> !t.isEmpty())
                    .forEach(t -> detectedFunctionalities.add(new Functionality(t.getAddedAttribute().getLocationInfo(), "modified attribute")));
        }
    }

    private void addOperations() {
        addOperationsFromClassDiffs(modelDiff.getCommonClassDiffList());
        addOperationsFromClassDiffs(modelDiff.getClassMoveDiffList());
        addOperationsFromClassDiffs(modelDiff.getClassRenameDiffList());

        //TODO check if classMergeDiffList and classSplitDiffList need to be added here
    }

    private void addOperationsFromClassDiffs(List<? extends UMLClassBaseDiff> classDiffs) {
        for (UMLClassBaseDiff classDiff : classDiffs) {
            classDiff.getAddedOperations().forEach(t -> detectedFunctionalities.add(new Functionality(t.getLocationInfo(), "added operation")));
        }
    }

    private void addStatements() {
        addStatementsFromClassDiffs(modelDiff.getCommonClassDiffList());
        addStatementsFromClassDiffs(modelDiff.getClassMoveDiffList());
        addStatementsFromClassDiffs(modelDiff.getClassRenameDiffList());

        //TODO check if classMergeDiffList and classSplitDiffList need to be added here
    }

    private void addStatementsFromClassDiffs(List<? extends UMLClassBaseDiff> classDiffs) {
        for (UMLClassBaseDiff classDiff : classDiffs) {
            for (UMLOperationBodyMapper umlOperationBodyMapper : classDiff.getOperationBodyMapperList()) {
                umlOperationBodyMapper.getNonMappedLeavesT2().forEach(t -> detectedFunctionalities.add(new Functionality(t.getLocationInfo(), "added statement")));

                for (AbstractCodeMapping mapping : umlOperationBodyMapper.getMappings()) {
                    if (mapping.getReplacements().isEmpty())
                        continue;

                    if (mapping.getFragment2() instanceof CompositeStatementObject) {
                        addModifiedCompositeStatement((CompositeStatementObject)mapping.getFragment2());
                        continue;
                    }

                    detectedFunctionalities.add(new Functionality(mapping.getFragment2().getLocationInfo(), "modified statement"));
                }
            }
        }
    }

    private void addModifiedCompositeStatement(CompositeStatementObject cs) {
        if (cs.getExpressions().isEmpty()) {
            LocationInfo loc = cs.getLocationInfo().copy();
            loc.reduceToFirstLine();

            detectedFunctionalities.add(new Functionality(loc, "modified composite statement condition"));
        } else {
            AbstractExpression lastExpr = cs.getExpressions().get(cs.getExpressions().size() - 1);

            LocationInfo loc = new LocationInfo(cs.getLocationInfo().getFilePath(),
                    cs.getLocationInfo().getStartOffset(),
                    lastExpr.getLocationInfo().getEndOffset(),
                    lastExpr.getLocationInfo().getEndOffset() - cs.getLocationInfo().getStartOffset(),
                    cs.getLocationInfo().getStartLine(),
                    cs.getLocationInfo().getStartColumn(),
                    lastExpr.getLocationInfo().getEndLine(),
                    lastExpr.getLocationInfo().getEndColumn(),
                    cs.getLocationInfo().getCodeElementType()
            );

            detectedFunctionalities.add(new Functionality(loc, "modified composite statement condition"));
        }
    }

    public List<Functionality> getDetectedFunctionalities() {
        return detectedFunctionalities;
    }
}
