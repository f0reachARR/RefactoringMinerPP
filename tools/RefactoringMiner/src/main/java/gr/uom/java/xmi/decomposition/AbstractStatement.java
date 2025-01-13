package gr.uom.java.xmi.decomposition;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonSubTypes;
import com.fasterxml.jackson.annotation.JsonTypeInfo;

import java.util.List;

@JsonTypeInfo(use = JsonTypeInfo.Id.NAME)
@JsonSubTypes({
		@JsonSubTypes.Type(value = CompositeStatementObject.class, name = "CompositeStatementObject"),
		@JsonSubTypes.Type(value = TryStatementObject.class, name = "TryStatementObject"),
		@JsonSubTypes.Type(value = StatementObject.class, name = "StatementObject")
})
public abstract class AbstractStatement extends AbstractCodeFragment {
	private CompositeStatementObject parent;
	
	public void setParent(CompositeStatementObject parent) {
    	this.parent = parent;
    }

    public CompositeStatementObject getParent() {
    	return this.parent;
    }

	public String getString() {
    	return toString();
    }

    public VariableDeclaration searchVariableDeclaration(String variableName) {
    	VariableDeclaration variableDeclaration = this.getVariableDeclaration(variableName);
    	if(variableDeclaration != null) {
    		return variableDeclaration;
    	}
    	else if(parent != null) {
    		return parent.searchVariableDeclaration(variableName);
    	}
    	return null;
    }

    public abstract List<AbstractCodeFragment> getLeaves();
    public abstract int statementCount();
	public abstract List<String> stringRepresentation();

	public AbstractStatement() {
	}

	@JsonCreator
	public AbstractStatement(
			@JsonProperty("id")int id,
			@JsonProperty("depth")int depth,
			@JsonProperty("index")int index,
			@JsonProperty("parent")int parent
	) {
		super(id, depth, index);

		this.parent = (CompositeStatementObject)AbstractCodeFragment.get(parent);
	}
}
