package gr.uom.java.xmi.decomposition;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.databind.DeserializationContext;
import com.fasterxml.jackson.databind.JsonDeserializer;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import gr.uom.java.xmi.LocationInfo;
import org.eclipse.jdt.core.dom.CompilationUnit;
import org.eclipse.jdt.core.dom.Statement;

import gr.uom.java.xmi.LocationInfo.CodeElementType;

@JsonDeserialize(using = TryStatementObject.TryStatementObjectDeserializer.class)
public class TryStatementObject extends CompositeStatementObject {
	private List<CompositeStatementObject> catchClauses;
	private CompositeStatementObject finallyClause;

	public TryStatementObject(CompilationUnit cu, String filePath, Statement statement, int depth) {
		super(cu, filePath, statement, depth, CodeElementType.TRY_STATEMENT);
		this.catchClauses = new ArrayList<CompositeStatementObject>();
	}

	public void addCatchClause(CompositeStatementObject catchClause) {
		catchClauses.add(catchClause);
	}

	public List<CompositeStatementObject> getCatchClauses() {
		return catchClauses;
	}

	public void setFinallyClause(CompositeStatementObject finallyClause) {
		this.finallyClause = finallyClause;
	}

	public CompositeStatementObject getFinallyClause() {
		return finallyClause;
	}

	public boolean isTryWithResources() {
		return getExpressions().size() > 0;
	}

	public boolean identicalCatchOrFinallyBlocks(TryStatementObject other) {
		int identicalCatchClauses = 0;
		boolean identicalFinallyClause = false;
		if(this.catchClauses.size() == other.catchClauses.size()) {
			for(int i=0; i<this.catchClauses.size(); i++) {
				CompositeStatementObject thisCatch = this.catchClauses.get(i);
				CompositeStatementObject otherCatch = other.catchClauses.get(i);
				if(thisCatch.stringRepresentation().equals(otherCatch.stringRepresentation())) {
					identicalCatchClauses++;
				}
			}
		}
		else {
			return false;
		}
		if(this.finallyClause != null && other.finallyClause != null) {
			if(this.finallyClause.stringRepresentation().equals(other.finallyClause.stringRepresentation())) {
				identicalFinallyClause = true;
			}
		}
		else if(this.finallyClause == null && other.finallyClause == null) {
			identicalFinallyClause = true;
		}
		return (identicalCatchClauses > 0 && identicalCatchClauses == this.catchClauses.size()) || identicalFinallyClause;
	}

	//json
	public TryStatementObject(
			int id,
			int depth,
			int index,
			int parent,
			LocationInfo locationInfo
	) {
		super(id, depth, index, parent, locationInfo);

		this.catchClauses = new ArrayList<>();
	}

	static class TryStatementObjectDeserializer extends JsonDeserializer<TryStatementObject> {
		@Override
		public TryStatementObject deserialize(JsonParser p, DeserializationContext ctxt) throws IOException {
			ObjectMapper om = new ObjectMapper();
			JsonNode node = ctxt.readTree(p);

			TryStatementObject stmt = new TryStatementObject(
					node.get("id").asInt(),
					node.get("depth").asInt(),
					node.get("index").asInt(),
					node.get("parent").asInt(),
					om.readValue(node.get("locationInfo").traverse(), LocationInfo.class)
			);

			List<AbstractStatement> statementList = om.readValue(node.get("statementList").traverse(), ctxt.getTypeFactory().constructCollectionType(List.class, AbstractStatement.class));
			List<AbstractExpression> expressionList = om.readValue(node.get("expressionList").traverse(), ctxt.getTypeFactory().constructCollectionType(List.class, AbstractExpression.class));
			List<VariableDeclaration> variableDeclarations = om.readValue(node.get("variableDeclarations").traverse(), ctxt.getTypeFactory().constructCollectionType(List.class, VariableDeclaration.class));

			java.util.Collections.reverse(statementList);
			java.util.Collections.reverse(expressionList);
			java.util.Collections.reverse(variableDeclarations);

			statementList.forEach(t -> stmt.getStatements().add(t));
			expressionList.forEach(t -> stmt.getExpressions().add(t));
			variableDeclarations.forEach(t -> stmt.getVariableDeclarations().add(t));

			stmt.getStatements().forEach(s -> s.getVariableDeclarations().forEach(v -> v.getScope().setParentSignature(stmt.getSignature())));
			stmt.getExpressions().forEach(e -> e.getVariableDeclarations().forEach(v -> v.getScope().setParentSignature(stmt.getSignature())));
			stmt.getVariableDeclarations().forEach(v -> v.getScope().setParentSignature(stmt.getSignature()));

			List<Integer> catchClauses = om.readValue(node.get("catchClauses").traverse(), ctxt.getTypeFactory().constructCollectionType(List.class, Integer.class));
			catchClauses.forEach(t ->
			{
				CompositeStatementObject catchClause = (CompositeStatementObject)AbstractCodeFragment.get(t);

				catchClause.setTryContainer(stmt);
				stmt.catchClauses.add(catchClause);
			});

			return stmt;
		}
	}
}
