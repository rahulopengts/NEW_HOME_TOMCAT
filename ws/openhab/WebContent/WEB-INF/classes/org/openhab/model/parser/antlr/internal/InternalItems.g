/*
* generated by Xtext
*/
grammar InternalItems;

options {
	superClass=AbstractInternalAntlrParser;
	
}

@lexer::header {
package org.openhab.model.parser.antlr.internal;

// Hack: Use our own Lexer superclass by means of import. 
// Currently there is no other way to specify the superclass for the lexer.
import org.eclipse.xtext.parser.antlr.Lexer;
}

@parser::header {
package org.openhab.model.parser.antlr.internal; 

import org.eclipse.xtext.*;
import org.eclipse.xtext.parser.*;
import org.eclipse.xtext.parser.impl.*;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.common.util.Enumerator;
import org.eclipse.xtext.parser.antlr.AbstractInternalAntlrParser;
import org.eclipse.xtext.parser.antlr.XtextTokenStream;
import org.eclipse.xtext.parser.antlr.XtextTokenStream.HiddenTokens;
import org.eclipse.xtext.parser.antlr.AntlrDatatypeRuleToken;
import org.openhab.model.services.ItemsGrammarAccess;

}

@parser::members {

 	private ItemsGrammarAccess grammarAccess;
 	
    public InternalItemsParser(TokenStream input, ItemsGrammarAccess grammarAccess) {
        this(input);
        this.grammarAccess = grammarAccess;
        registerRules(grammarAccess.getGrammar());
    }
    
    @Override
    protected String getFirstRuleName() {
    	return "ItemModel";	
   	}
   	
   	@Override
   	protected ItemsGrammarAccess getGrammarAccess() {
   		return grammarAccess;
   	}
}

@rulecatch { 
    catch (RecognitionException re) { 
        recover(input,re); 
        appendSkippedTokens();
    } 
}




// Entry rule entryRuleItemModel
entryRuleItemModel returns [EObject current=null] 
	:
	{ newCompositeNode(grammarAccess.getItemModelRule()); }
	 iv_ruleItemModel=ruleItemModel 
	 { $current=$iv_ruleItemModel.current; } 
	 EOF 
;

// Rule ItemModel
ruleItemModel returns [EObject current=null] 
    @init { enterRule(); 
    }
    @after { leaveRule(); }:
((
    {
        $current = forceCreateModelElement(
            grammarAccess.getItemModelAccess().getItemModelAction_0(),
            $current);
    }
)(
(
		{ 
	        newCompositeNode(grammarAccess.getItemModelAccess().getItemsModelItemParserRuleCall_1_0()); 
	    }
		lv_items_1_0=ruleModelItem		{
	        if ($current==null) {
	            $current = createModelElementForParent(grammarAccess.getItemModelRule());
	        }
       		add(
       			$current, 
       			"items",
        		lv_items_1_0, 
        		"ModelItem");
	        afterParserOrEnumRuleCall();
	    }

)
)*)
;





// Entry rule entryRuleModelItem
entryRuleModelItem returns [EObject current=null] 
	:
	{ newCompositeNode(grammarAccess.getModelItemRule()); }
	 iv_ruleModelItem=ruleModelItem 
	 { $current=$iv_ruleModelItem.current; } 
	 EOF 
;

// Rule ModelItem
ruleModelItem returns [EObject current=null] 
    @init { enterRule(); 
    }
    @after { leaveRule(); }:
((
    { 
        newCompositeNode(grammarAccess.getModelItemAccess().getModelNormalItemParserRuleCall_0_0()); 
    }
    this_ModelNormalItem_0=ruleModelNormalItem
    { 
        $current = $this_ModelNormalItem_0.current; 
        afterParserOrEnumRuleCall();
    }

    |
    { 
        newCompositeNode(grammarAccess.getModelItemAccess().getModelGroupItemParserRuleCall_0_1()); 
    }
    this_ModelGroupItem_1=ruleModelGroupItem
    { 
        $current = $this_ModelGroupItem_1.current; 
        afterParserOrEnumRuleCall();
    }
)(
(
		lv_name_2_0=RULE_ID
		{
			newLeafNode(lv_name_2_0, grammarAccess.getModelItemAccess().getNameIDTerminalRuleCall_1_0()); 
		}
		{
	        if ($current==null) {
	            $current = createModelElement(grammarAccess.getModelItemRule());
	        }
       		setWithLastConsumed(
       			$current, 
       			"name",
        		lv_name_2_0, 
        		"ID");
	    }

)
)(
(
		lv_label_3_0=RULE_STRING
		{
			newLeafNode(lv_label_3_0, grammarAccess.getModelItemAccess().getLabelSTRINGTerminalRuleCall_2_0()); 
		}
		{
	        if ($current==null) {
	            $current = createModelElement(grammarAccess.getModelItemRule());
	        }
       		setWithLastConsumed(
       			$current, 
       			"label",
        		lv_label_3_0, 
        		"STRING");
	    }

)
)?(	otherlv_4='<' 
    {
    	newLeafNode(otherlv_4, grammarAccess.getModelItemAccess().getLessThanSignKeyword_3_0());
    }
(
(
(
		lv_icon_5_1=RULE_ID
		{
			newLeafNode(lv_icon_5_1, grammarAccess.getModelItemAccess().getIconIDTerminalRuleCall_3_1_0_0()); 
		}
		{
	        if ($current==null) {
	            $current = createModelElement(grammarAccess.getModelItemRule());
	        }
       		setWithLastConsumed(
       			$current, 
       			"icon",
        		lv_icon_5_1, 
        		"ID");
	    }

    |		lv_icon_5_2=RULE_STRING
		{
			newLeafNode(lv_icon_5_2, grammarAccess.getModelItemAccess().getIconSTRINGTerminalRuleCall_3_1_0_1()); 
		}
		{
	        if ($current==null) {
	            $current = createModelElement(grammarAccess.getModelItemRule());
	        }
       		setWithLastConsumed(
       			$current, 
       			"icon",
        		lv_icon_5_2, 
        		"STRING");
	    }

)

)
)	otherlv_6='>' 
    {
    	newLeafNode(otherlv_6, grammarAccess.getModelItemAccess().getGreaterThanSignKeyword_3_2());
    }
)?(	otherlv_7='(' 
    {
    	newLeafNode(otherlv_7, grammarAccess.getModelItemAccess().getLeftParenthesisKeyword_4_0());
    }
(
(
		lv_groups_8_0=RULE_ID
		{
			newLeafNode(lv_groups_8_0, grammarAccess.getModelItemAccess().getGroupsIDTerminalRuleCall_4_1_0()); 
		}
		{
	        if ($current==null) {
	            $current = createModelElement(grammarAccess.getModelItemRule());
	        }
       		addWithLastConsumed(
       			$current, 
       			"groups",
        		lv_groups_8_0, 
        		"ID");
	    }

)
)(	otherlv_9=',' 
    {
    	newLeafNode(otherlv_9, grammarAccess.getModelItemAccess().getCommaKeyword_4_2_0());
    }
(
(
		lv_groups_10_0=RULE_ID
		{
			newLeafNode(lv_groups_10_0, grammarAccess.getModelItemAccess().getGroupsIDTerminalRuleCall_4_2_1_0()); 
		}
		{
	        if ($current==null) {
	            $current = createModelElement(grammarAccess.getModelItemRule());
	        }
       		addWithLastConsumed(
       			$current, 
       			"groups",
        		lv_groups_10_0, 
        		"ID");
	    }

)
))*	otherlv_11=')' 
    {
    	newLeafNode(otherlv_11, grammarAccess.getModelItemAccess().getRightParenthesisKeyword_4_3());
    }
)?(	otherlv_12='{' 
    {
    	newLeafNode(otherlv_12, grammarAccess.getModelItemAccess().getLeftCurlyBracketKeyword_5_0());
    }
(
(
		{ 
	        newCompositeNode(grammarAccess.getModelItemAccess().getBindingsModelBindingParserRuleCall_5_1_0()); 
	    }
		lv_bindings_13_0=ruleModelBinding		{
	        if ($current==null) {
	            $current = createModelElementForParent(grammarAccess.getModelItemRule());
	        }
       		add(
       			$current, 
       			"bindings",
        		lv_bindings_13_0, 
        		"ModelBinding");
	        afterParserOrEnumRuleCall();
	    }

)
)(	otherlv_14=',' 
    {
    	newLeafNode(otherlv_14, grammarAccess.getModelItemAccess().getCommaKeyword_5_2_0());
    }
(
(
		{ 
	        newCompositeNode(grammarAccess.getModelItemAccess().getBindingsModelBindingParserRuleCall_5_2_1_0()); 
	    }
		lv_bindings_15_0=ruleModelBinding		{
	        if ($current==null) {
	            $current = createModelElementForParent(grammarAccess.getModelItemRule());
	        }
       		add(
       			$current, 
       			"bindings",
        		lv_bindings_15_0, 
        		"ModelBinding");
	        afterParserOrEnumRuleCall();
	    }

)
))*	otherlv_16='}' 
    {
    	newLeafNode(otherlv_16, grammarAccess.getModelItemAccess().getRightCurlyBracketKeyword_5_3());
    }
)?)
;





// Entry rule entryRuleModelGroupItem
entryRuleModelGroupItem returns [EObject current=null] 
	:
	{ newCompositeNode(grammarAccess.getModelGroupItemRule()); }
	 iv_ruleModelGroupItem=ruleModelGroupItem 
	 { $current=$iv_ruleModelGroupItem.current; } 
	 EOF 
;

// Rule ModelGroupItem
ruleModelGroupItem returns [EObject current=null] 
    @init { enterRule(); 
    }
    @after { leaveRule(); }:
((
    {
        $current = forceCreateModelElement(
            grammarAccess.getModelGroupItemAccess().getModelGroupItemAction_0(),
            $current);
    }
)	otherlv_1='Group' 
    {
    	newLeafNode(otherlv_1, grammarAccess.getModelGroupItemAccess().getGroupKeyword_1());
    }
(	otherlv_2=':' 
    {
    	newLeafNode(otherlv_2, grammarAccess.getModelGroupItemAccess().getColonKeyword_2_0());
    }
(
(
		{ 
	        newCompositeNode(grammarAccess.getModelGroupItemAccess().getTypeModelItemTypeParserRuleCall_2_1_0()); 
	    }
		lv_type_3_0=ruleModelItemType		{
	        if ($current==null) {
	            $current = createModelElementForParent(grammarAccess.getModelGroupItemRule());
	        }
       		set(
       			$current, 
       			"type",
        		lv_type_3_0, 
        		"ModelItemType");
	        afterParserOrEnumRuleCall();
	    }

)
)(	otherlv_4=':' 
    {
    	newLeafNode(otherlv_4, grammarAccess.getModelGroupItemAccess().getColonKeyword_2_2_0());
    }
(
(
		{ 
	        newCompositeNode(grammarAccess.getModelGroupItemAccess().getFunctionModelGroupFunctionEnumRuleCall_2_2_1_0()); 
	    }
		lv_function_5_0=ruleModelGroupFunction		{
	        if ($current==null) {
	            $current = createModelElementForParent(grammarAccess.getModelGroupItemRule());
	        }
       		set(
       			$current, 
       			"function",
        		lv_function_5_0, 
        		"ModelGroupFunction");
	        afterParserOrEnumRuleCall();
	    }

)
)(	otherlv_6='(' 
    {
    	newLeafNode(otherlv_6, grammarAccess.getModelGroupItemAccess().getLeftParenthesisKeyword_2_2_2_0());
    }
(
(
(
		lv_args_7_1=RULE_ID
		{
			newLeafNode(lv_args_7_1, grammarAccess.getModelGroupItemAccess().getArgsIDTerminalRuleCall_2_2_2_1_0_0()); 
		}
		{
	        if ($current==null) {
	            $current = createModelElement(grammarAccess.getModelGroupItemRule());
	        }
       		addWithLastConsumed(
       			$current, 
       			"args",
        		lv_args_7_1, 
        		"ID");
	    }

    |		lv_args_7_2=RULE_STRING
		{
			newLeafNode(lv_args_7_2, grammarAccess.getModelGroupItemAccess().getArgsSTRINGTerminalRuleCall_2_2_2_1_0_1()); 
		}
		{
	        if ($current==null) {
	            $current = createModelElement(grammarAccess.getModelGroupItemRule());
	        }
       		addWithLastConsumed(
       			$current, 
       			"args",
        		lv_args_7_2, 
        		"STRING");
	    }

)

)
)(	otherlv_8=',' 
    {
    	newLeafNode(otherlv_8, grammarAccess.getModelGroupItemAccess().getCommaKeyword_2_2_2_2_0());
    }
(
(
(
		lv_args_9_1=RULE_ID
		{
			newLeafNode(lv_args_9_1, grammarAccess.getModelGroupItemAccess().getArgsIDTerminalRuleCall_2_2_2_2_1_0_0()); 
		}
		{
	        if ($current==null) {
	            $current = createModelElement(grammarAccess.getModelGroupItemRule());
	        }
       		addWithLastConsumed(
       			$current, 
       			"args",
        		lv_args_9_1, 
        		"ID");
	    }

    |		lv_args_9_2=RULE_STRING
		{
			newLeafNode(lv_args_9_2, grammarAccess.getModelGroupItemAccess().getArgsSTRINGTerminalRuleCall_2_2_2_2_1_0_1()); 
		}
		{
	        if ($current==null) {
	            $current = createModelElement(grammarAccess.getModelGroupItemRule());
	        }
       		addWithLastConsumed(
       			$current, 
       			"args",
        		lv_args_9_2, 
        		"STRING");
	    }

)

)
))*	otherlv_10=')' 
    {
    	newLeafNode(otherlv_10, grammarAccess.getModelGroupItemAccess().getRightParenthesisKeyword_2_2_2_3());
    }
)?)?)?)
;





// Entry rule entryRuleModelNormalItem
entryRuleModelNormalItem returns [EObject current=null] 
	:
	{ newCompositeNode(grammarAccess.getModelNormalItemRule()); }
	 iv_ruleModelNormalItem=ruleModelNormalItem 
	 { $current=$iv_ruleModelNormalItem.current; } 
	 EOF 
;

// Rule ModelNormalItem
ruleModelNormalItem returns [EObject current=null] 
    @init { enterRule(); 
    }
    @after { leaveRule(); }:
(
(
		{ 
	        newCompositeNode(grammarAccess.getModelNormalItemAccess().getTypeModelItemTypeParserRuleCall_0()); 
	    }
		lv_type_0_0=ruleModelItemType		{
	        if ($current==null) {
	            $current = createModelElementForParent(grammarAccess.getModelNormalItemRule());
	        }
       		set(
       			$current, 
       			"type",
        		lv_type_0_0, 
        		"ModelItemType");
	        afterParserOrEnumRuleCall();
	    }

)
)
;





// Entry rule entryRuleModelItemType
entryRuleModelItemType returns [String current=null] 
	:
	{ newCompositeNode(grammarAccess.getModelItemTypeRule()); } 
	 iv_ruleModelItemType=ruleModelItemType 
	 { $current=$iv_ruleModelItemType.current.getText(); }  
	 EOF 
;

// Rule ModelItemType
ruleModelItemType returns [AntlrDatatypeRuleToken current=new AntlrDatatypeRuleToken()] 
    @init { enterRule(); 
    }
    @after { leaveRule(); }:
(
	kw='Switch' 
    {
        $current.merge(kw);
        newLeafNode(kw, grammarAccess.getModelItemTypeAccess().getSwitchKeyword_0()); 
    }

    |
	kw='Rollershutter' 
    {
        $current.merge(kw);
        newLeafNode(kw, grammarAccess.getModelItemTypeAccess().getRollershutterKeyword_1()); 
    }

    |
	kw='Number' 
    {
        $current.merge(kw);
        newLeafNode(kw, grammarAccess.getModelItemTypeAccess().getNumberKeyword_2()); 
    }

    |
	kw='String' 
    {
        $current.merge(kw);
        newLeafNode(kw, grammarAccess.getModelItemTypeAccess().getStringKeyword_3()); 
    }

    |
	kw='Dimmer' 
    {
        $current.merge(kw);
        newLeafNode(kw, grammarAccess.getModelItemTypeAccess().getDimmerKeyword_4()); 
    }

    |
	kw='Contact' 
    {
        $current.merge(kw);
        newLeafNode(kw, grammarAccess.getModelItemTypeAccess().getContactKeyword_5()); 
    }

    |
	kw='DateTime' 
    {
        $current.merge(kw);
        newLeafNode(kw, grammarAccess.getModelItemTypeAccess().getDateTimeKeyword_6()); 
    }

    |
	kw='Color' 
    {
        $current.merge(kw);
        newLeafNode(kw, grammarAccess.getModelItemTypeAccess().getColorKeyword_7()); 
    }

    |
	kw='Point' 
    {
        $current.merge(kw);
        newLeafNode(kw, grammarAccess.getModelItemTypeAccess().getPointKeyword_8()); 
    }

    |    this_ID_9=RULE_ID    {
		$current.merge(this_ID_9);
    }

    { 
    newLeafNode(this_ID_9, grammarAccess.getModelItemTypeAccess().getIDTerminalRuleCall_9()); 
    }
)
    ;





// Entry rule entryRuleModelBinding
entryRuleModelBinding returns [EObject current=null] 
	:
	{ newCompositeNode(grammarAccess.getModelBindingRule()); }
	 iv_ruleModelBinding=ruleModelBinding 
	 { $current=$iv_ruleModelBinding.current; } 
	 EOF 
;

// Rule ModelBinding
ruleModelBinding returns [EObject current=null] 
    @init { enterRule(); 
    }
    @after { leaveRule(); }:
((
(
		lv_type_0_0=RULE_ID
		{
			newLeafNode(lv_type_0_0, grammarAccess.getModelBindingAccess().getTypeIDTerminalRuleCall_0_0()); 
		}
		{
	        if ($current==null) {
	            $current = createModelElement(grammarAccess.getModelBindingRule());
	        }
       		setWithLastConsumed(
       			$current, 
       			"type",
        		lv_type_0_0, 
        		"ID");
	    }

)
)	otherlv_1='=' 
    {
    	newLeafNode(otherlv_1, grammarAccess.getModelBindingAccess().getEqualsSignKeyword_1());
    }
(
(
		lv_configuration_2_0=RULE_STRING
		{
			newLeafNode(lv_configuration_2_0, grammarAccess.getModelBindingAccess().getConfigurationSTRINGTerminalRuleCall_2_0()); 
		}
		{
	        if ($current==null) {
	            $current = createModelElement(grammarAccess.getModelBindingRule());
	        }
       		setWithLastConsumed(
       			$current, 
       			"configuration",
        		lv_configuration_2_0, 
        		"STRING");
	    }

)
))
;





// Rule ModelGroupFunction
ruleModelGroupFunction returns [Enumerator current=null] 
    @init { enterRule(); }
    @after { leaveRule(); }:
((	enumLiteral_0='AND' 
	{
        $current = grammarAccess.getModelGroupFunctionAccess().getANDEnumLiteralDeclaration_0().getEnumLiteral().getInstance();
        newLeafNode(enumLiteral_0, grammarAccess.getModelGroupFunctionAccess().getANDEnumLiteralDeclaration_0()); 
    }
)
    |(	enumLiteral_1='OR' 
	{
        $current = grammarAccess.getModelGroupFunctionAccess().getOREnumLiteralDeclaration_1().getEnumLiteral().getInstance();
        newLeafNode(enumLiteral_1, grammarAccess.getModelGroupFunctionAccess().getOREnumLiteralDeclaration_1()); 
    }
)
    |(	enumLiteral_2='NAND' 
	{
        $current = grammarAccess.getModelGroupFunctionAccess().getNANDEnumLiteralDeclaration_2().getEnumLiteral().getInstance();
        newLeafNode(enumLiteral_2, grammarAccess.getModelGroupFunctionAccess().getNANDEnumLiteralDeclaration_2()); 
    }
)
    |(	enumLiteral_3='NOR' 
	{
        $current = grammarAccess.getModelGroupFunctionAccess().getNOREnumLiteralDeclaration_3().getEnumLiteral().getInstance();
        newLeafNode(enumLiteral_3, grammarAccess.getModelGroupFunctionAccess().getNOREnumLiteralDeclaration_3()); 
    }
)
    |(	enumLiteral_4='AVG' 
	{
        $current = grammarAccess.getModelGroupFunctionAccess().getAVGEnumLiteralDeclaration_4().getEnumLiteral().getInstance();
        newLeafNode(enumLiteral_4, grammarAccess.getModelGroupFunctionAccess().getAVGEnumLiteralDeclaration_4()); 
    }
)
    |(	enumLiteral_5='SUM' 
	{
        $current = grammarAccess.getModelGroupFunctionAccess().getSUMEnumLiteralDeclaration_5().getEnumLiteral().getInstance();
        newLeafNode(enumLiteral_5, grammarAccess.getModelGroupFunctionAccess().getSUMEnumLiteralDeclaration_5()); 
    }
)
    |(	enumLiteral_6='MAX' 
	{
        $current = grammarAccess.getModelGroupFunctionAccess().getMAXEnumLiteralDeclaration_6().getEnumLiteral().getInstance();
        newLeafNode(enumLiteral_6, grammarAccess.getModelGroupFunctionAccess().getMAXEnumLiteralDeclaration_6()); 
    }
)
    |(	enumLiteral_7='MIN' 
	{
        $current = grammarAccess.getModelGroupFunctionAccess().getMINEnumLiteralDeclaration_7().getEnumLiteral().getInstance();
        newLeafNode(enumLiteral_7, grammarAccess.getModelGroupFunctionAccess().getMINEnumLiteralDeclaration_7()); 
    }
)
    |(	enumLiteral_8='COUNT' 
	{
        $current = grammarAccess.getModelGroupFunctionAccess().getCOUNTEnumLiteralDeclaration_8().getEnumLiteral().getInstance();
        newLeafNode(enumLiteral_8, grammarAccess.getModelGroupFunctionAccess().getCOUNTEnumLiteralDeclaration_8()); 
    }
));



RULE_ID : '^'? ('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'_'|'0'..'9')*;

RULE_INT : ('0'..'9')+;

RULE_STRING : ('"' ('\\' ('b'|'t'|'n'|'f'|'r'|'u'|'"'|'\''|'\\')|~(('\\'|'"')))* '"'|'\'' ('\\' ('b'|'t'|'n'|'f'|'r'|'u'|'"'|'\''|'\\')|~(('\\'|'\'')))* '\'');

RULE_ML_COMMENT : '/*' ( options {greedy=false;} : . )*'*/';

RULE_SL_COMMENT : '//' ~(('\n'|'\r'))* ('\r'? '\n')?;

RULE_WS : (' '|'\t'|'\r'|'\n')+;

RULE_ANY_OTHER : .;

