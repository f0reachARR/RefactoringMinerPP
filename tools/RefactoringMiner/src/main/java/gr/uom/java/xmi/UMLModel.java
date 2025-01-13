package gr.uom.java.xmi;

import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.databind.DeserializationContext;
import com.fasterxml.jackson.databind.JsonDeserializer;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import gr.uom.java.xmi.diff.UMLClassDiff;
import gr.uom.java.xmi.diff.UMLModelDiff;

import java.io.IOException;
import java.util.*;

import org.refactoringminer.api.RefactoringMinerTimedOutException;

import com.github.gumtreediff.tree.TreeContext;

@JsonDeserialize(using = UMLModelDeserializer.class)
public class UMLModel {
	private Set<String> repositoryDirectories;
    private List<UMLClass> classList;
    private List<UMLGeneralization> generalizationList;
    private List<UMLRealization> realizationList;
    private boolean partial;
    private Map<String, TreeContext> treeContextMap = new LinkedHashMap<>();
    private Map<String, List<UMLComment>> commentMap = new LinkedHashMap<>();

    public UMLModel(Set<String> repositoryDirectories) {
    	this.repositoryDirectories = repositoryDirectories;
        classList = new ArrayList<UMLClass>();
        generalizationList = new ArrayList<UMLGeneralization>();
        realizationList = new ArrayList<UMLRealization>();
    }

	public Map<String, TreeContext> getTreeContextMap() {
		return treeContextMap;
	}

	public Map<String, List<UMLComment>> getCommentMap() {
		return commentMap;
	}

	public boolean isPartial() {
		return partial;
	}

	public void setPartial(boolean partial) {
		this.partial = partial;
	}

	public void addClass(UMLClass umlClass) {
        classList.add(umlClass);
    }

    public void addGeneralization(UMLGeneralization umlGeneralization) {
        generalizationList.add(umlGeneralization);
    }

    public void addRealization(UMLRealization umlRealization) {
    	realizationList.add(umlRealization);
    }

    public UMLClass getClass(UMLClass umlClassFromOtherModel) {
    	ListIterator<UMLClass> it = classList.listIterator();
        while(it.hasNext()) {
            UMLClass umlClass = it.next();
            if(umlClass.equals(umlClassFromOtherModel))
                return umlClass;
        }
        return null;
    }

    public List<UMLClass> getClassList() {
        return this.classList;
    }

    public List<UMLGeneralization> getGeneralizationList() {
        return this.generalizationList;
    }

    public List<UMLRealization> getRealizationList() {
		return realizationList;
	}

	public UMLGeneralization matchGeneralization(UMLGeneralization otherGeneralization) {
    	ListIterator<UMLGeneralization> generalizationIt = generalizationList.listIterator();
    	while(generalizationIt.hasNext()) {
    		UMLGeneralization generalization = generalizationIt.next();
    		if(generalization.getChild().equals(otherGeneralization.getChild())) {
    			String thisParent = generalization.getParent();
    			String otherParent = otherGeneralization.getParent();
    			String thisParentComparedString = null;
    			if(thisParent.contains("."))
    				thisParentComparedString = thisParent.substring(thisParent.lastIndexOf(".")+1);
    			else
    				thisParentComparedString = thisParent;
    			String otherParentComparedString = null;
    			if(otherParent.contains("."))
    				otherParentComparedString = otherParent.substring(otherParent.lastIndexOf(".")+1);
    			else
    				otherParentComparedString = otherParent;
    			if(thisParentComparedString.equals(otherParentComparedString))
    				return generalization;
    		}
    	}
    	return null;
    }

    public UMLRealization matchRealization(UMLRealization otherRealization) {
    	ListIterator<UMLRealization> realizationIt = realizationList.listIterator();
    	while(realizationIt.hasNext()) {
    		UMLRealization realization = realizationIt.next();
    		if(realization.getClient().equals(otherRealization.getClient())) {
    			String thisSupplier = realization.getSupplier();
    			String otherSupplier = otherRealization.getSupplier();
    			String thisSupplierComparedString = null;
    			if(thisSupplier.contains("."))
    				thisSupplierComparedString = thisSupplier.substring(thisSupplier.lastIndexOf(".")+1);
    			else
    				thisSupplierComparedString = thisSupplier;
    			String otherSupplierComparedString = null;
    			if(otherSupplier.contains("."))
    				otherSupplierComparedString = otherSupplier.substring(otherSupplier.lastIndexOf(".")+1);
    			else
    				otherSupplierComparedString = otherSupplier;
    			if(thisSupplierComparedString.equals(otherSupplierComparedString))
    				return realization;
    		}
    	}
    	return null;
    }

	public UMLModelDiff diff(UMLModel umlModel) throws RefactoringMinerTimedOutException {
    	UMLModelDiff modelDiff = new UMLModelDiff(this, umlModel);
    	for(UMLClass umlClass : classList) {
    		if(!umlModel.classList.contains(umlClass))
    			modelDiff.reportRemovedClass(umlClass);
    	}
    	for(UMLClass umlClass : umlModel.classList) {
    		if(!this.classList.contains(umlClass))
    			modelDiff.reportAddedClass(umlClass);
    	}
    	modelDiff.checkForMovedClasses(umlModel.repositoryDirectories, new UMLClassMatcher.Move());
    	modelDiff.checkForRenamedClasses(new UMLClassMatcher.Rename());
    	for(UMLGeneralization umlGeneralization : generalizationList) {
    		if(!umlModel.generalizationList.contains(umlGeneralization))
    			modelDiff.reportRemovedGeneralization(umlGeneralization);
    	}
    	for(UMLGeneralization umlGeneralization : umlModel.generalizationList) {
    		if(!this.generalizationList.contains(umlGeneralization))
    			modelDiff.reportAddedGeneralization(umlGeneralization);
    	}
    	modelDiff.checkForGeneralizationChanges();
    	for(UMLRealization umlRealization : realizationList) {
    		if(!umlModel.realizationList.contains(umlRealization))
    			modelDiff.reportRemovedRealization(umlRealization);
    	}
    	for(UMLRealization umlRealization : umlModel.realizationList) {
    		if(!this.realizationList.contains(umlRealization))
    			modelDiff.reportAddedRealization(umlRealization);
    	}
    	modelDiff.checkForRealizationChanges();
    	for(UMLClass umlClass : classList) {
    		if(umlModel.classList.contains(umlClass)) {
    			UMLClassDiff classDiff = new UMLClassDiff(umlClass, umlModel.getClass(umlClass), modelDiff);
    			classDiff.process();
    			modelDiff.addUMLClassDiff(classDiff);
    		}
    	}
    	modelDiff.checkForMovedClasses(umlModel.repositoryDirectories, new UMLClassMatcher.RelaxedMove());
    	modelDiff.checkForRenamedClasses(new UMLClassMatcher.RelaxedRename());
    	modelDiff.inferClassRenameBasedOnFilePaths(new UMLClassMatcher.RelaxedRename());
    	return modelDiff;
    }

	//json
	public UMLModel(
			List<String> repositoryDirectories,
			List<UMLClass> classList,
			List<UMLGeneralization> generalizationList,
			List<UMLRealization> realizationList,
			boolean partial
	) {
		this.repositoryDirectories = new HashSet<>(repositoryDirectories);
		this.classList = classList;
		this.generalizationList = generalizationList;
		this.realizationList = realizationList;
		this.partial = partial;
	}
}

class UMLModelDeserializer extends JsonDeserializer<UMLModel> {
	@Override
	public UMLModel deserialize(JsonParser p, DeserializationContext ctxt) throws IOException {
		ObjectMapper om = new ObjectMapper();
		JsonNode node = ctxt.readTree(p);

		return new UMLModel(
				om.readValue(node.get("repositoryDirectories").traverse(), ctxt.getTypeFactory().constructCollectionType(List.class, String.class)),
				om.readValue(node.get("classList").traverse(), ctxt.getTypeFactory().constructCollectionType(List.class, UMLClass.class)),
				om.readValue(node.get("generalizationList").traverse(), ctxt.getTypeFactory().constructCollectionType(List.class, UMLGeneralization.class)),
				om.readValue(node.get("realizationList").traverse(), ctxt.getTypeFactory().constructCollectionType(List.class, UMLRealization.class)),
				node.get("partial").asBoolean()
		);
	}
}
