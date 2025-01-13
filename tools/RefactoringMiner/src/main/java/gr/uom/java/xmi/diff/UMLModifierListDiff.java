package gr.uom.java.xmi.diff;

import gr.uom.java.xmi.UMLModifier;

import java.util.ArrayList;
import java.util.List;

public class UMLModifierListDiff {
    public static List<UMLModifier> getAddedModifiers(List<UMLModifier> parentModifiers, List<UMLModifier> currentModifiers) {
        List<UMLModifier> oldModifiers = filterModifierList(new ArrayList<>(parentModifiers));
        List<UMLModifier> newModifiers = filterModifierList(new ArrayList<>(currentModifiers));

        for (UMLModifier om : oldModifiers) {
            for (UMLModifier nm : newModifiers) {
                if (om.getKeyword().equals(nm.getKeyword())) {
                    newModifiers.remove(nm);
                    break;
                }
            }
        }

        return newModifiers;
    }

    public static List<UMLModifier> getRemovedModifiers(List<UMLModifier> parentModifiers, List<UMLModifier> currentModifiers) {
        List<UMLModifier> oldModifiers = filterModifierList(new ArrayList<>(parentModifiers));
        List<UMLModifier> newModifiers = filterModifierList(new ArrayList<>(currentModifiers));

        for (UMLModifier nm : newModifiers) {
            for (UMLModifier om : oldModifiers) {
                if (om.getKeyword().equals(nm.getKeyword())) {
                    oldModifiers.remove(om);
                    break;
                }
            }
        }

        return oldModifiers;
    }

    private static List<UMLModifier> filterModifierList(List<UMLModifier> modifiers) {
        modifiers.removeIf(t -> t.getKeyword().equals("public")
            || t.getKeyword().equals("private")
            || t.getKeyword().equals("protected"));

        return modifiers;
    }
}
