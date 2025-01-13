package gr.uom.java.xmi;

import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.databind.DeserializationContext;
import com.fasterxml.jackson.databind.JsonDeserializer;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import gr.uom.java.xmi.decomposition.VariableDeclaration;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

@JsonDeserialize(using = UMLEnumConstantDeserializer.class)
public class UMLEnumConstant extends UMLAttribute {
	private List<String> arguments;

	public UMLEnumConstant(String name, UMLType type, LocationInfo locationInfo) {
		super(name, type, locationInfo);
		this.arguments = new ArrayList<String>();
	}

	public void addArgument(String argument) {
		this.arguments.add(argument);
	}

	public List<String> getArguments() {
		return arguments;
	}

    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append(getName());
        if(arguments.size() > 0) {
        	sb.append("(");
            for(int i=0; i<arguments.size()-1; i++) {
                sb.append(arguments.get(i)).append(", ");
            }
            sb.append(arguments.get(arguments.size()-1));
            sb.append(")");
        }
        return sb.toString();
    }

	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + ((getName() == null) ? 0 : getName().hashCode());
		return result;
	}

	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		UMLEnumConstant other = (UMLEnumConstant) obj;
		if (getName() == null) {
			if (other.getName() != null)
				return false;
		} else if (!getName().equals(other.getName()))
			return false;
		return true;
	}

	//json
	public UMLEnumConstant(
			int id,
			LocationInfo locationInfo,
			String name,
			UMLType type,
			Visibility visibility,
			String className,
			boolean isFinal,
			boolean isStatic,
			boolean isVolatile,
			List<String> arguments
	) {
		super(id, locationInfo, name, type, visibility, className, isFinal, isStatic, isVolatile);

		this.arguments = arguments;
	}
}

class UMLEnumConstantDeserializer extends JsonDeserializer<UMLEnumConstant> {
	@Override
	public UMLEnumConstant deserialize(JsonParser p, DeserializationContext ctxt) throws IOException {
		ObjectMapper om = new ObjectMapper();
		JsonNode node = ctxt.readTree(p);

		UMLEnumConstant e = new UMLEnumConstant(
				node.get("id").asInt(),
				om.readValue(node.get("locationInfo").traverse(), LocationInfo.class),
				node.get("name").textValue(),
				om.readValue(node.get("type").traverse(), UMLType.class),
				om.readValue(node.get("visibility").traverse(), Visibility.class),
				node.get("className").textValue(),
				node.get("isFinal").asBoolean(),
				node.get("isStatic").asBoolean(),
				node.get("isVolatile").asBoolean(),
				om.readValue(node.get("arguments").traverse(), ctxt.getTypeFactory().constructCollectionType(List.class, String.class))
		);

		e.setVariableDeclaration(om.readValue(node.get("variableDeclaration").traverse(), VariableDeclaration.class));

		return e;
	}
}
