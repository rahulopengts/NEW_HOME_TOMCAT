package org.openhab.model.persistence.parser.antlr.internal; 

import org.eclipse.xtext.*;
import org.eclipse.xtext.parser.*;
import org.eclipse.xtext.parser.impl.*;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.xtext.parser.antlr.AbstractInternalAntlrParser;
import org.eclipse.xtext.parser.antlr.XtextTokenStream;
import org.eclipse.xtext.parser.antlr.XtextTokenStream.HiddenTokens;
import org.eclipse.xtext.parser.antlr.AntlrDatatypeRuleToken;
import org.openhab.model.persistence.services.PersistenceGrammarAccess;



import org.antlr.runtime.*;
import java.util.Stack;
import java.util.List;
import java.util.ArrayList;

@SuppressWarnings("all")
public class InternalPersistenceParser extends AbstractInternalAntlrParser {
    public static final String[] tokenNames = new String[] {
        "<invalid>", "<EOR>", "<DOWN>", "<UP>", "RULE_ID", "RULE_STRING", "RULE_INT", "RULE_ML_COMMENT", "RULE_SL_COMMENT", "RULE_WS", "RULE_ANY_OTHER", "'Strategies'", "'{'", "'default'", "'='", "','", "'}'", "'Filters'", "'Items'", "':'", "'>'", "'%'", "'s'", "'m'", "'h'", "'d'", "'->'", "'strategy'", "'filter'", "';'", "'*'", "'.'"
    };
    public static final int RULE_ID=4;
    public static final int T__29=29;
    public static final int T__28=28;
    public static final int T__27=27;
    public static final int T__26=26;
    public static final int T__25=25;
    public static final int T__24=24;
    public static final int T__23=23;
    public static final int T__22=22;
    public static final int RULE_ANY_OTHER=10;
    public static final int T__21=21;
    public static final int T__20=20;
    public static final int RULE_SL_COMMENT=8;
    public static final int EOF=-1;
    public static final int RULE_ML_COMMENT=7;
    public static final int T__30=30;
    public static final int T__19=19;
    public static final int T__31=31;
    public static final int RULE_STRING=5;
    public static final int T__16=16;
    public static final int T__15=15;
    public static final int T__18=18;
    public static final int T__17=17;
    public static final int T__12=12;
    public static final int T__11=11;
    public static final int T__14=14;
    public static final int T__13=13;
    public static final int RULE_INT=6;
    public static final int RULE_WS=9;

    // delegates
    // delegators


        public InternalPersistenceParser(TokenStream input) {
            this(input, new RecognizerSharedState());
        }
        public InternalPersistenceParser(TokenStream input, RecognizerSharedState state) {
            super(input, state);
             
        }
        

    public String[] getTokenNames() { return InternalPersistenceParser.tokenNames; }
    public String getGrammarFileName() { return "../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g"; }



     	private PersistenceGrammarAccess grammarAccess;
     	
        public InternalPersistenceParser(TokenStream input, PersistenceGrammarAccess grammarAccess) {
            this(input);
            this.grammarAccess = grammarAccess;
            registerRules(grammarAccess.getGrammar());
        }
        
        @Override
        protected String getFirstRuleName() {
        	return "PersistenceModel";	
       	}
       	
       	@Override
       	protected PersistenceGrammarAccess getGrammarAccess() {
       		return grammarAccess;
       	}



    // $ANTLR start "entryRulePersistenceModel"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:67:1: entryRulePersistenceModel returns [EObject current=null] : iv_rulePersistenceModel= rulePersistenceModel EOF ;
    public final EObject entryRulePersistenceModel() throws RecognitionException {
        EObject current = null;

        EObject iv_rulePersistenceModel = null;


        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:68:2: (iv_rulePersistenceModel= rulePersistenceModel EOF )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:69:2: iv_rulePersistenceModel= rulePersistenceModel EOF
            {
             newCompositeNode(grammarAccess.getPersistenceModelRule()); 
            pushFollow(FOLLOW_rulePersistenceModel_in_entryRulePersistenceModel75);
            iv_rulePersistenceModel=rulePersistenceModel();

            state._fsp--;

             current =iv_rulePersistenceModel; 
            match(input,EOF,FOLLOW_EOF_in_entryRulePersistenceModel85); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "entryRulePersistenceModel"


    // $ANTLR start "rulePersistenceModel"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:76:1: rulePersistenceModel returns [EObject current=null] : ( () otherlv_1= 'Strategies' otherlv_2= '{' ( (lv_strategies_3_0= ruleStrategy ) )* (otherlv_4= 'default' otherlv_5= '=' ( (otherlv_6= RULE_ID ) ) (otherlv_7= ',' ( (otherlv_8= RULE_ID ) ) )* )? otherlv_9= '}' (otherlv_10= 'Filters' otherlv_11= '{' ( (lv_filters_12_0= ruleFilter ) )* otherlv_13= '}' )? (otherlv_14= 'Items' otherlv_15= '{' ( (lv_configs_16_0= rulePersistenceConfiguration ) )* otherlv_17= '}' )? ) ;
    public final EObject rulePersistenceModel() throws RecognitionException {
        EObject current = null;

        Token otherlv_1=null;
        Token otherlv_2=null;
        Token otherlv_4=null;
        Token otherlv_5=null;
        Token otherlv_6=null;
        Token otherlv_7=null;
        Token otherlv_8=null;
        Token otherlv_9=null;
        Token otherlv_10=null;
        Token otherlv_11=null;
        Token otherlv_13=null;
        Token otherlv_14=null;
        Token otherlv_15=null;
        Token otherlv_17=null;
        EObject lv_strategies_3_0 = null;

        EObject lv_filters_12_0 = null;

        EObject lv_configs_16_0 = null;


         enterRule(); 
            
        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:79:28: ( ( () otherlv_1= 'Strategies' otherlv_2= '{' ( (lv_strategies_3_0= ruleStrategy ) )* (otherlv_4= 'default' otherlv_5= '=' ( (otherlv_6= RULE_ID ) ) (otherlv_7= ',' ( (otherlv_8= RULE_ID ) ) )* )? otherlv_9= '}' (otherlv_10= 'Filters' otherlv_11= '{' ( (lv_filters_12_0= ruleFilter ) )* otherlv_13= '}' )? (otherlv_14= 'Items' otherlv_15= '{' ( (lv_configs_16_0= rulePersistenceConfiguration ) )* otherlv_17= '}' )? ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:80:1: ( () otherlv_1= 'Strategies' otherlv_2= '{' ( (lv_strategies_3_0= ruleStrategy ) )* (otherlv_4= 'default' otherlv_5= '=' ( (otherlv_6= RULE_ID ) ) (otherlv_7= ',' ( (otherlv_8= RULE_ID ) ) )* )? otherlv_9= '}' (otherlv_10= 'Filters' otherlv_11= '{' ( (lv_filters_12_0= ruleFilter ) )* otherlv_13= '}' )? (otherlv_14= 'Items' otherlv_15= '{' ( (lv_configs_16_0= rulePersistenceConfiguration ) )* otherlv_17= '}' )? )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:80:1: ( () otherlv_1= 'Strategies' otherlv_2= '{' ( (lv_strategies_3_0= ruleStrategy ) )* (otherlv_4= 'default' otherlv_5= '=' ( (otherlv_6= RULE_ID ) ) (otherlv_7= ',' ( (otherlv_8= RULE_ID ) ) )* )? otherlv_9= '}' (otherlv_10= 'Filters' otherlv_11= '{' ( (lv_filters_12_0= ruleFilter ) )* otherlv_13= '}' )? (otherlv_14= 'Items' otherlv_15= '{' ( (lv_configs_16_0= rulePersistenceConfiguration ) )* otherlv_17= '}' )? )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:80:2: () otherlv_1= 'Strategies' otherlv_2= '{' ( (lv_strategies_3_0= ruleStrategy ) )* (otherlv_4= 'default' otherlv_5= '=' ( (otherlv_6= RULE_ID ) ) (otherlv_7= ',' ( (otherlv_8= RULE_ID ) ) )* )? otherlv_9= '}' (otherlv_10= 'Filters' otherlv_11= '{' ( (lv_filters_12_0= ruleFilter ) )* otherlv_13= '}' )? (otherlv_14= 'Items' otherlv_15= '{' ( (lv_configs_16_0= rulePersistenceConfiguration ) )* otherlv_17= '}' )?
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:80:2: ()
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:81:5: 
            {

                    current = forceCreateModelElement(
                        grammarAccess.getPersistenceModelAccess().getPersistenceModelAction_0(),
                        current);
                

            }

            otherlv_1=(Token)match(input,11,FOLLOW_11_in_rulePersistenceModel131); 

                	newLeafNode(otherlv_1, grammarAccess.getPersistenceModelAccess().getStrategiesKeyword_1());
                
            otherlv_2=(Token)match(input,12,FOLLOW_12_in_rulePersistenceModel143); 

                	newLeafNode(otherlv_2, grammarAccess.getPersistenceModelAccess().getLeftCurlyBracketKeyword_2());
                
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:94:1: ( (lv_strategies_3_0= ruleStrategy ) )*
            loop1:
            do {
                int alt1=2;
                int LA1_0 = input.LA(1);

                if ( (LA1_0==RULE_ID) ) {
                    alt1=1;
                }


                switch (alt1) {
            	case 1 :
            	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:95:1: (lv_strategies_3_0= ruleStrategy )
            	    {
            	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:95:1: (lv_strategies_3_0= ruleStrategy )
            	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:96:3: lv_strategies_3_0= ruleStrategy
            	    {
            	     
            	    	        newCompositeNode(grammarAccess.getPersistenceModelAccess().getStrategiesStrategyParserRuleCall_3_0()); 
            	    	    
            	    pushFollow(FOLLOW_ruleStrategy_in_rulePersistenceModel164);
            	    lv_strategies_3_0=ruleStrategy();

            	    state._fsp--;


            	    	        if (current==null) {
            	    	            current = createModelElementForParent(grammarAccess.getPersistenceModelRule());
            	    	        }
            	           		add(
            	           			current, 
            	           			"strategies",
            	            		lv_strategies_3_0, 
            	            		"Strategy");
            	    	        afterParserOrEnumRuleCall();
            	    	    

            	    }


            	    }
            	    break;

            	default :
            	    break loop1;
                }
            } while (true);

            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:112:3: (otherlv_4= 'default' otherlv_5= '=' ( (otherlv_6= RULE_ID ) ) (otherlv_7= ',' ( (otherlv_8= RULE_ID ) ) )* )?
            int alt3=2;
            int LA3_0 = input.LA(1);

            if ( (LA3_0==13) ) {
                alt3=1;
            }
            switch (alt3) {
                case 1 :
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:112:5: otherlv_4= 'default' otherlv_5= '=' ( (otherlv_6= RULE_ID ) ) (otherlv_7= ',' ( (otherlv_8= RULE_ID ) ) )*
                    {
                    otherlv_4=(Token)match(input,13,FOLLOW_13_in_rulePersistenceModel178); 

                        	newLeafNode(otherlv_4, grammarAccess.getPersistenceModelAccess().getDefaultKeyword_4_0());
                        
                    otherlv_5=(Token)match(input,14,FOLLOW_14_in_rulePersistenceModel190); 

                        	newLeafNode(otherlv_5, grammarAccess.getPersistenceModelAccess().getEqualsSignKeyword_4_1());
                        
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:120:1: ( (otherlv_6= RULE_ID ) )
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:121:1: (otherlv_6= RULE_ID )
                    {
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:121:1: (otherlv_6= RULE_ID )
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:122:3: otherlv_6= RULE_ID
                    {

                    			if (current==null) {
                    	            current = createModelElement(grammarAccess.getPersistenceModelRule());
                    	        }
                            
                    otherlv_6=(Token)match(input,RULE_ID,FOLLOW_RULE_ID_in_rulePersistenceModel210); 

                    		newLeafNode(otherlv_6, grammarAccess.getPersistenceModelAccess().getDefaultsStrategyCrossReference_4_2_0()); 
                    	

                    }


                    }

                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:133:2: (otherlv_7= ',' ( (otherlv_8= RULE_ID ) ) )*
                    loop2:
                    do {
                        int alt2=2;
                        int LA2_0 = input.LA(1);

                        if ( (LA2_0==15) ) {
                            alt2=1;
                        }


                        switch (alt2) {
                    	case 1 :
                    	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:133:4: otherlv_7= ',' ( (otherlv_8= RULE_ID ) )
                    	    {
                    	    otherlv_7=(Token)match(input,15,FOLLOW_15_in_rulePersistenceModel223); 

                    	        	newLeafNode(otherlv_7, grammarAccess.getPersistenceModelAccess().getCommaKeyword_4_3_0());
                    	        
                    	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:137:1: ( (otherlv_8= RULE_ID ) )
                    	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:138:1: (otherlv_8= RULE_ID )
                    	    {
                    	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:138:1: (otherlv_8= RULE_ID )
                    	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:139:3: otherlv_8= RULE_ID
                    	    {

                    	    			if (current==null) {
                    	    	            current = createModelElement(grammarAccess.getPersistenceModelRule());
                    	    	        }
                    	            
                    	    otherlv_8=(Token)match(input,RULE_ID,FOLLOW_RULE_ID_in_rulePersistenceModel243); 

                    	    		newLeafNode(otherlv_8, grammarAccess.getPersistenceModelAccess().getDefaultsStrategyCrossReference_4_3_1_0()); 
                    	    	

                    	    }


                    	    }


                    	    }
                    	    break;

                    	default :
                    	    break loop2;
                        }
                    } while (true);


                    }
                    break;

            }

            otherlv_9=(Token)match(input,16,FOLLOW_16_in_rulePersistenceModel259); 

                	newLeafNode(otherlv_9, grammarAccess.getPersistenceModelAccess().getRightCurlyBracketKeyword_5());
                
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:154:1: (otherlv_10= 'Filters' otherlv_11= '{' ( (lv_filters_12_0= ruleFilter ) )* otherlv_13= '}' )?
            int alt5=2;
            int LA5_0 = input.LA(1);

            if ( (LA5_0==17) ) {
                alt5=1;
            }
            switch (alt5) {
                case 1 :
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:154:3: otherlv_10= 'Filters' otherlv_11= '{' ( (lv_filters_12_0= ruleFilter ) )* otherlv_13= '}'
                    {
                    otherlv_10=(Token)match(input,17,FOLLOW_17_in_rulePersistenceModel272); 

                        	newLeafNode(otherlv_10, grammarAccess.getPersistenceModelAccess().getFiltersKeyword_6_0());
                        
                    otherlv_11=(Token)match(input,12,FOLLOW_12_in_rulePersistenceModel284); 

                        	newLeafNode(otherlv_11, grammarAccess.getPersistenceModelAccess().getLeftCurlyBracketKeyword_6_1());
                        
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:162:1: ( (lv_filters_12_0= ruleFilter ) )*
                    loop4:
                    do {
                        int alt4=2;
                        int LA4_0 = input.LA(1);

                        if ( (LA4_0==RULE_ID) ) {
                            alt4=1;
                        }


                        switch (alt4) {
                    	case 1 :
                    	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:163:1: (lv_filters_12_0= ruleFilter )
                    	    {
                    	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:163:1: (lv_filters_12_0= ruleFilter )
                    	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:164:3: lv_filters_12_0= ruleFilter
                    	    {
                    	     
                    	    	        newCompositeNode(grammarAccess.getPersistenceModelAccess().getFiltersFilterParserRuleCall_6_2_0()); 
                    	    	    
                    	    pushFollow(FOLLOW_ruleFilter_in_rulePersistenceModel305);
                    	    lv_filters_12_0=ruleFilter();

                    	    state._fsp--;


                    	    	        if (current==null) {
                    	    	            current = createModelElementForParent(grammarAccess.getPersistenceModelRule());
                    	    	        }
                    	           		add(
                    	           			current, 
                    	           			"filters",
                    	            		lv_filters_12_0, 
                    	            		"Filter");
                    	    	        afterParserOrEnumRuleCall();
                    	    	    

                    	    }


                    	    }
                    	    break;

                    	default :
                    	    break loop4;
                        }
                    } while (true);

                    otherlv_13=(Token)match(input,16,FOLLOW_16_in_rulePersistenceModel318); 

                        	newLeafNode(otherlv_13, grammarAccess.getPersistenceModelAccess().getRightCurlyBracketKeyword_6_3());
                        

                    }
                    break;

            }

            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:184:3: (otherlv_14= 'Items' otherlv_15= '{' ( (lv_configs_16_0= rulePersistenceConfiguration ) )* otherlv_17= '}' )?
            int alt7=2;
            int LA7_0 = input.LA(1);

            if ( (LA7_0==18) ) {
                alt7=1;
            }
            switch (alt7) {
                case 1 :
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:184:5: otherlv_14= 'Items' otherlv_15= '{' ( (lv_configs_16_0= rulePersistenceConfiguration ) )* otherlv_17= '}'
                    {
                    otherlv_14=(Token)match(input,18,FOLLOW_18_in_rulePersistenceModel333); 

                        	newLeafNode(otherlv_14, grammarAccess.getPersistenceModelAccess().getItemsKeyword_7_0());
                        
                    otherlv_15=(Token)match(input,12,FOLLOW_12_in_rulePersistenceModel345); 

                        	newLeafNode(otherlv_15, grammarAccess.getPersistenceModelAccess().getLeftCurlyBracketKeyword_7_1());
                        
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:192:1: ( (lv_configs_16_0= rulePersistenceConfiguration ) )*
                    loop6:
                    do {
                        int alt6=2;
                        int LA6_0 = input.LA(1);

                        if ( (LA6_0==RULE_ID||LA6_0==30) ) {
                            alt6=1;
                        }


                        switch (alt6) {
                    	case 1 :
                    	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:193:1: (lv_configs_16_0= rulePersistenceConfiguration )
                    	    {
                    	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:193:1: (lv_configs_16_0= rulePersistenceConfiguration )
                    	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:194:3: lv_configs_16_0= rulePersistenceConfiguration
                    	    {
                    	     
                    	    	        newCompositeNode(grammarAccess.getPersistenceModelAccess().getConfigsPersistenceConfigurationParserRuleCall_7_2_0()); 
                    	    	    
                    	    pushFollow(FOLLOW_rulePersistenceConfiguration_in_rulePersistenceModel366);
                    	    lv_configs_16_0=rulePersistenceConfiguration();

                    	    state._fsp--;


                    	    	        if (current==null) {
                    	    	            current = createModelElementForParent(grammarAccess.getPersistenceModelRule());
                    	    	        }
                    	           		add(
                    	           			current, 
                    	           			"configs",
                    	            		lv_configs_16_0, 
                    	            		"PersistenceConfiguration");
                    	    	        afterParserOrEnumRuleCall();
                    	    	    

                    	    }


                    	    }
                    	    break;

                    	default :
                    	    break loop6;
                        }
                    } while (true);

                    otherlv_17=(Token)match(input,16,FOLLOW_16_in_rulePersistenceModel379); 

                        	newLeafNode(otherlv_17, grammarAccess.getPersistenceModelAccess().getRightCurlyBracketKeyword_7_3());
                        

                    }
                    break;

            }


            }


            }

             leaveRule(); 
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "rulePersistenceModel"


    // $ANTLR start "entryRuleStrategy"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:222:1: entryRuleStrategy returns [EObject current=null] : iv_ruleStrategy= ruleStrategy EOF ;
    public final EObject entryRuleStrategy() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleStrategy = null;


        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:223:2: (iv_ruleStrategy= ruleStrategy EOF )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:224:2: iv_ruleStrategy= ruleStrategy EOF
            {
             newCompositeNode(grammarAccess.getStrategyRule()); 
            pushFollow(FOLLOW_ruleStrategy_in_entryRuleStrategy417);
            iv_ruleStrategy=ruleStrategy();

            state._fsp--;

             current =iv_ruleStrategy; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleStrategy427); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "entryRuleStrategy"


    // $ANTLR start "ruleStrategy"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:231:1: ruleStrategy returns [EObject current=null] : (this_CronStrategy_0= ruleCronStrategy | ( (lv_name_1_0= RULE_ID ) ) ) ;
    public final EObject ruleStrategy() throws RecognitionException {
        EObject current = null;

        Token lv_name_1_0=null;
        EObject this_CronStrategy_0 = null;


         enterRule(); 
            
        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:234:28: ( (this_CronStrategy_0= ruleCronStrategy | ( (lv_name_1_0= RULE_ID ) ) ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:235:1: (this_CronStrategy_0= ruleCronStrategy | ( (lv_name_1_0= RULE_ID ) ) )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:235:1: (this_CronStrategy_0= ruleCronStrategy | ( (lv_name_1_0= RULE_ID ) ) )
            int alt8=2;
            int LA8_0 = input.LA(1);

            if ( (LA8_0==RULE_ID) ) {
                int LA8_1 = input.LA(2);

                if ( (LA8_1==EOF||LA8_1==RULE_ID||LA8_1==13||LA8_1==16) ) {
                    alt8=2;
                }
                else if ( (LA8_1==19) ) {
                    alt8=1;
                }
                else {
                    NoViableAltException nvae =
                        new NoViableAltException("", 8, 1, input);

                    throw nvae;
                }
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 8, 0, input);

                throw nvae;
            }
            switch (alt8) {
                case 1 :
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:236:5: this_CronStrategy_0= ruleCronStrategy
                    {
                     
                            newCompositeNode(grammarAccess.getStrategyAccess().getCronStrategyParserRuleCall_0()); 
                        
                    pushFollow(FOLLOW_ruleCronStrategy_in_ruleStrategy474);
                    this_CronStrategy_0=ruleCronStrategy();

                    state._fsp--;

                     
                            current = this_CronStrategy_0; 
                            afterParserOrEnumRuleCall();
                        

                    }
                    break;
                case 2 :
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:245:6: ( (lv_name_1_0= RULE_ID ) )
                    {
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:245:6: ( (lv_name_1_0= RULE_ID ) )
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:246:1: (lv_name_1_0= RULE_ID )
                    {
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:246:1: (lv_name_1_0= RULE_ID )
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:247:3: lv_name_1_0= RULE_ID
                    {
                    lv_name_1_0=(Token)match(input,RULE_ID,FOLLOW_RULE_ID_in_ruleStrategy496); 

                    			newLeafNode(lv_name_1_0, grammarAccess.getStrategyAccess().getNameIDTerminalRuleCall_1_0()); 
                    		

                    	        if (current==null) {
                    	            current = createModelElement(grammarAccess.getStrategyRule());
                    	        }
                           		setWithLastConsumed(
                           			current, 
                           			"name",
                            		lv_name_1_0, 
                            		"ID");
                    	    

                    }


                    }


                    }
                    break;

            }


            }

             leaveRule(); 
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "ruleStrategy"


    // $ANTLR start "entryRuleCronStrategy"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:271:1: entryRuleCronStrategy returns [EObject current=null] : iv_ruleCronStrategy= ruleCronStrategy EOF ;
    public final EObject entryRuleCronStrategy() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleCronStrategy = null;


        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:272:2: (iv_ruleCronStrategy= ruleCronStrategy EOF )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:273:2: iv_ruleCronStrategy= ruleCronStrategy EOF
            {
             newCompositeNode(grammarAccess.getCronStrategyRule()); 
            pushFollow(FOLLOW_ruleCronStrategy_in_entryRuleCronStrategy537);
            iv_ruleCronStrategy=ruleCronStrategy();

            state._fsp--;

             current =iv_ruleCronStrategy; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleCronStrategy547); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "entryRuleCronStrategy"


    // $ANTLR start "ruleCronStrategy"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:280:1: ruleCronStrategy returns [EObject current=null] : ( () ( (lv_name_1_0= RULE_ID ) ) otherlv_2= ':' ( (lv_cronExpression_3_0= RULE_STRING ) ) ) ;
    public final EObject ruleCronStrategy() throws RecognitionException {
        EObject current = null;

        Token lv_name_1_0=null;
        Token otherlv_2=null;
        Token lv_cronExpression_3_0=null;

         enterRule(); 
            
        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:283:28: ( ( () ( (lv_name_1_0= RULE_ID ) ) otherlv_2= ':' ( (lv_cronExpression_3_0= RULE_STRING ) ) ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:284:1: ( () ( (lv_name_1_0= RULE_ID ) ) otherlv_2= ':' ( (lv_cronExpression_3_0= RULE_STRING ) ) )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:284:1: ( () ( (lv_name_1_0= RULE_ID ) ) otherlv_2= ':' ( (lv_cronExpression_3_0= RULE_STRING ) ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:284:2: () ( (lv_name_1_0= RULE_ID ) ) otherlv_2= ':' ( (lv_cronExpression_3_0= RULE_STRING ) )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:284:2: ()
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:285:5: 
            {

                    current = forceCreateModelElement(
                        grammarAccess.getCronStrategyAccess().getCronStrategyAction_0(),
                        current);
                

            }

            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:290:2: ( (lv_name_1_0= RULE_ID ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:291:1: (lv_name_1_0= RULE_ID )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:291:1: (lv_name_1_0= RULE_ID )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:292:3: lv_name_1_0= RULE_ID
            {
            lv_name_1_0=(Token)match(input,RULE_ID,FOLLOW_RULE_ID_in_ruleCronStrategy598); 

            			newLeafNode(lv_name_1_0, grammarAccess.getCronStrategyAccess().getNameIDTerminalRuleCall_1_0()); 
            		

            	        if (current==null) {
            	            current = createModelElement(grammarAccess.getCronStrategyRule());
            	        }
                   		setWithLastConsumed(
                   			current, 
                   			"name",
                    		lv_name_1_0, 
                    		"ID");
            	    

            }


            }

            otherlv_2=(Token)match(input,19,FOLLOW_19_in_ruleCronStrategy615); 

                	newLeafNode(otherlv_2, grammarAccess.getCronStrategyAccess().getColonKeyword_2());
                
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:312:1: ( (lv_cronExpression_3_0= RULE_STRING ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:313:1: (lv_cronExpression_3_0= RULE_STRING )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:313:1: (lv_cronExpression_3_0= RULE_STRING )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:314:3: lv_cronExpression_3_0= RULE_STRING
            {
            lv_cronExpression_3_0=(Token)match(input,RULE_STRING,FOLLOW_RULE_STRING_in_ruleCronStrategy632); 

            			newLeafNode(lv_cronExpression_3_0, grammarAccess.getCronStrategyAccess().getCronExpressionSTRINGTerminalRuleCall_3_0()); 
            		

            	        if (current==null) {
            	            current = createModelElement(grammarAccess.getCronStrategyRule());
            	        }
                   		setWithLastConsumed(
                   			current, 
                   			"cronExpression",
                    		lv_cronExpression_3_0, 
                    		"STRING");
            	    

            }


            }


            }


            }

             leaveRule(); 
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "ruleCronStrategy"


    // $ANTLR start "entryRuleFilter"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:338:1: entryRuleFilter returns [EObject current=null] : iv_ruleFilter= ruleFilter EOF ;
    public final EObject entryRuleFilter() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleFilter = null;


        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:339:2: (iv_ruleFilter= ruleFilter EOF )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:340:2: iv_ruleFilter= ruleFilter EOF
            {
             newCompositeNode(grammarAccess.getFilterRule()); 
            pushFollow(FOLLOW_ruleFilter_in_entryRuleFilter673);
            iv_ruleFilter=ruleFilter();

            state._fsp--;

             current =iv_ruleFilter; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleFilter683); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "entryRuleFilter"


    // $ANTLR start "ruleFilter"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:347:1: ruleFilter returns [EObject current=null] : ( ( (lv_name_0_0= RULE_ID ) ) otherlv_1= ':' ( (lv_definition_2_0= ruleFilterDetails ) ) ) ;
    public final EObject ruleFilter() throws RecognitionException {
        EObject current = null;

        Token lv_name_0_0=null;
        Token otherlv_1=null;
        EObject lv_definition_2_0 = null;


         enterRule(); 
            
        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:350:28: ( ( ( (lv_name_0_0= RULE_ID ) ) otherlv_1= ':' ( (lv_definition_2_0= ruleFilterDetails ) ) ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:351:1: ( ( (lv_name_0_0= RULE_ID ) ) otherlv_1= ':' ( (lv_definition_2_0= ruleFilterDetails ) ) )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:351:1: ( ( (lv_name_0_0= RULE_ID ) ) otherlv_1= ':' ( (lv_definition_2_0= ruleFilterDetails ) ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:351:2: ( (lv_name_0_0= RULE_ID ) ) otherlv_1= ':' ( (lv_definition_2_0= ruleFilterDetails ) )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:351:2: ( (lv_name_0_0= RULE_ID ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:352:1: (lv_name_0_0= RULE_ID )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:352:1: (lv_name_0_0= RULE_ID )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:353:3: lv_name_0_0= RULE_ID
            {
            lv_name_0_0=(Token)match(input,RULE_ID,FOLLOW_RULE_ID_in_ruleFilter725); 

            			newLeafNode(lv_name_0_0, grammarAccess.getFilterAccess().getNameIDTerminalRuleCall_0_0()); 
            		

            	        if (current==null) {
            	            current = createModelElement(grammarAccess.getFilterRule());
            	        }
                   		setWithLastConsumed(
                   			current, 
                   			"name",
                    		lv_name_0_0, 
                    		"ID");
            	    

            }


            }

            otherlv_1=(Token)match(input,19,FOLLOW_19_in_ruleFilter742); 

                	newLeafNode(otherlv_1, grammarAccess.getFilterAccess().getColonKeyword_1());
                
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:373:1: ( (lv_definition_2_0= ruleFilterDetails ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:374:1: (lv_definition_2_0= ruleFilterDetails )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:374:1: (lv_definition_2_0= ruleFilterDetails )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:375:3: lv_definition_2_0= ruleFilterDetails
            {
             
            	        newCompositeNode(grammarAccess.getFilterAccess().getDefinitionFilterDetailsParserRuleCall_2_0()); 
            	    
            pushFollow(FOLLOW_ruleFilterDetails_in_ruleFilter763);
            lv_definition_2_0=ruleFilterDetails();

            state._fsp--;


            	        if (current==null) {
            	            current = createModelElementForParent(grammarAccess.getFilterRule());
            	        }
                   		set(
                   			current, 
                   			"definition",
                    		lv_definition_2_0, 
                    		"FilterDetails");
            	        afterParserOrEnumRuleCall();
            	    

            }


            }


            }


            }

             leaveRule(); 
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "ruleFilter"


    // $ANTLR start "entryRuleFilterDetails"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:399:1: entryRuleFilterDetails returns [EObject current=null] : iv_ruleFilterDetails= ruleFilterDetails EOF ;
    public final EObject entryRuleFilterDetails() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleFilterDetails = null;


        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:400:2: (iv_ruleFilterDetails= ruleFilterDetails EOF )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:401:2: iv_ruleFilterDetails= ruleFilterDetails EOF
            {
             newCompositeNode(grammarAccess.getFilterDetailsRule()); 
            pushFollow(FOLLOW_ruleFilterDetails_in_entryRuleFilterDetails799);
            iv_ruleFilterDetails=ruleFilterDetails();

            state._fsp--;

             current =iv_ruleFilterDetails; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleFilterDetails809); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "entryRuleFilterDetails"


    // $ANTLR start "ruleFilterDetails"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:408:1: ruleFilterDetails returns [EObject current=null] : (this_ThresholdFilter_0= ruleThresholdFilter | this_TimeFilter_1= ruleTimeFilter ) ;
    public final EObject ruleFilterDetails() throws RecognitionException {
        EObject current = null;

        EObject this_ThresholdFilter_0 = null;

        EObject this_TimeFilter_1 = null;


         enterRule(); 
            
        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:411:28: ( (this_ThresholdFilter_0= ruleThresholdFilter | this_TimeFilter_1= ruleTimeFilter ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:412:1: (this_ThresholdFilter_0= ruleThresholdFilter | this_TimeFilter_1= ruleTimeFilter )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:412:1: (this_ThresholdFilter_0= ruleThresholdFilter | this_TimeFilter_1= ruleTimeFilter )
            int alt9=2;
            int LA9_0 = input.LA(1);

            if ( (LA9_0==20) ) {
                alt9=1;
            }
            else if ( (LA9_0==RULE_INT) ) {
                alt9=2;
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 9, 0, input);

                throw nvae;
            }
            switch (alt9) {
                case 1 :
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:413:5: this_ThresholdFilter_0= ruleThresholdFilter
                    {
                     
                            newCompositeNode(grammarAccess.getFilterDetailsAccess().getThresholdFilterParserRuleCall_0()); 
                        
                    pushFollow(FOLLOW_ruleThresholdFilter_in_ruleFilterDetails856);
                    this_ThresholdFilter_0=ruleThresholdFilter();

                    state._fsp--;

                     
                            current = this_ThresholdFilter_0; 
                            afterParserOrEnumRuleCall();
                        

                    }
                    break;
                case 2 :
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:423:5: this_TimeFilter_1= ruleTimeFilter
                    {
                     
                            newCompositeNode(grammarAccess.getFilterDetailsAccess().getTimeFilterParserRuleCall_1()); 
                        
                    pushFollow(FOLLOW_ruleTimeFilter_in_ruleFilterDetails883);
                    this_TimeFilter_1=ruleTimeFilter();

                    state._fsp--;

                     
                            current = this_TimeFilter_1; 
                            afterParserOrEnumRuleCall();
                        

                    }
                    break;

            }


            }

             leaveRule(); 
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "ruleFilterDetails"


    // $ANTLR start "entryRuleThresholdFilter"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:439:1: entryRuleThresholdFilter returns [EObject current=null] : iv_ruleThresholdFilter= ruleThresholdFilter EOF ;
    public final EObject entryRuleThresholdFilter() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleThresholdFilter = null;


        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:440:2: (iv_ruleThresholdFilter= ruleThresholdFilter EOF )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:441:2: iv_ruleThresholdFilter= ruleThresholdFilter EOF
            {
             newCompositeNode(grammarAccess.getThresholdFilterRule()); 
            pushFollow(FOLLOW_ruleThresholdFilter_in_entryRuleThresholdFilter918);
            iv_ruleThresholdFilter=ruleThresholdFilter();

            state._fsp--;

             current =iv_ruleThresholdFilter; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleThresholdFilter928); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "entryRuleThresholdFilter"


    // $ANTLR start "ruleThresholdFilter"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:448:1: ruleThresholdFilter returns [EObject current=null] : (otherlv_0= '>' ( (lv_value_1_0= ruleDECIMAL ) ) ( (lv_percent_2_0= '%' ) ) ) ;
    public final EObject ruleThresholdFilter() throws RecognitionException {
        EObject current = null;

        Token otherlv_0=null;
        Token lv_percent_2_0=null;
        AntlrDatatypeRuleToken lv_value_1_0 = null;


         enterRule(); 
            
        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:451:28: ( (otherlv_0= '>' ( (lv_value_1_0= ruleDECIMAL ) ) ( (lv_percent_2_0= '%' ) ) ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:452:1: (otherlv_0= '>' ( (lv_value_1_0= ruleDECIMAL ) ) ( (lv_percent_2_0= '%' ) ) )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:452:1: (otherlv_0= '>' ( (lv_value_1_0= ruleDECIMAL ) ) ( (lv_percent_2_0= '%' ) ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:452:3: otherlv_0= '>' ( (lv_value_1_0= ruleDECIMAL ) ) ( (lv_percent_2_0= '%' ) )
            {
            otherlv_0=(Token)match(input,20,FOLLOW_20_in_ruleThresholdFilter965); 

                	newLeafNode(otherlv_0, grammarAccess.getThresholdFilterAccess().getGreaterThanSignKeyword_0());
                
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:456:1: ( (lv_value_1_0= ruleDECIMAL ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:457:1: (lv_value_1_0= ruleDECIMAL )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:457:1: (lv_value_1_0= ruleDECIMAL )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:458:3: lv_value_1_0= ruleDECIMAL
            {
             
            	        newCompositeNode(grammarAccess.getThresholdFilterAccess().getValueDECIMALParserRuleCall_1_0()); 
            	    
            pushFollow(FOLLOW_ruleDECIMAL_in_ruleThresholdFilter986);
            lv_value_1_0=ruleDECIMAL();

            state._fsp--;


            	        if (current==null) {
            	            current = createModelElementForParent(grammarAccess.getThresholdFilterRule());
            	        }
                   		set(
                   			current, 
                   			"value",
                    		lv_value_1_0, 
                    		"DECIMAL");
            	        afterParserOrEnumRuleCall();
            	    

            }


            }

            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:474:2: ( (lv_percent_2_0= '%' ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:475:1: (lv_percent_2_0= '%' )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:475:1: (lv_percent_2_0= '%' )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:476:3: lv_percent_2_0= '%'
            {
            lv_percent_2_0=(Token)match(input,21,FOLLOW_21_in_ruleThresholdFilter1004); 

                    newLeafNode(lv_percent_2_0, grammarAccess.getThresholdFilterAccess().getPercentPercentSignKeyword_2_0());
                

            	        if (current==null) {
            	            current = createModelElement(grammarAccess.getThresholdFilterRule());
            	        }
                   		setWithLastConsumed(current, "percent", true, "%");
            	    

            }


            }


            }


            }

             leaveRule(); 
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "ruleThresholdFilter"


    // $ANTLR start "entryRuleTimeFilter"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:497:1: entryRuleTimeFilter returns [EObject current=null] : iv_ruleTimeFilter= ruleTimeFilter EOF ;
    public final EObject entryRuleTimeFilter() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleTimeFilter = null;


        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:498:2: (iv_ruleTimeFilter= ruleTimeFilter EOF )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:499:2: iv_ruleTimeFilter= ruleTimeFilter EOF
            {
             newCompositeNode(grammarAccess.getTimeFilterRule()); 
            pushFollow(FOLLOW_ruleTimeFilter_in_entryRuleTimeFilter1053);
            iv_ruleTimeFilter=ruleTimeFilter();

            state._fsp--;

             current =iv_ruleTimeFilter; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleTimeFilter1063); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "entryRuleTimeFilter"


    // $ANTLR start "ruleTimeFilter"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:506:1: ruleTimeFilter returns [EObject current=null] : ( ( (lv_value_0_0= RULE_INT ) ) ( ( (lv_unit_1_1= 's' | lv_unit_1_2= 'm' | lv_unit_1_3= 'h' | lv_unit_1_4= 'd' ) ) ) ) ;
    public final EObject ruleTimeFilter() throws RecognitionException {
        EObject current = null;

        Token lv_value_0_0=null;
        Token lv_unit_1_1=null;
        Token lv_unit_1_2=null;
        Token lv_unit_1_3=null;
        Token lv_unit_1_4=null;

         enterRule(); 
            
        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:509:28: ( ( ( (lv_value_0_0= RULE_INT ) ) ( ( (lv_unit_1_1= 's' | lv_unit_1_2= 'm' | lv_unit_1_3= 'h' | lv_unit_1_4= 'd' ) ) ) ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:510:1: ( ( (lv_value_0_0= RULE_INT ) ) ( ( (lv_unit_1_1= 's' | lv_unit_1_2= 'm' | lv_unit_1_3= 'h' | lv_unit_1_4= 'd' ) ) ) )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:510:1: ( ( (lv_value_0_0= RULE_INT ) ) ( ( (lv_unit_1_1= 's' | lv_unit_1_2= 'm' | lv_unit_1_3= 'h' | lv_unit_1_4= 'd' ) ) ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:510:2: ( (lv_value_0_0= RULE_INT ) ) ( ( (lv_unit_1_1= 's' | lv_unit_1_2= 'm' | lv_unit_1_3= 'h' | lv_unit_1_4= 'd' ) ) )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:510:2: ( (lv_value_0_0= RULE_INT ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:511:1: (lv_value_0_0= RULE_INT )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:511:1: (lv_value_0_0= RULE_INT )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:512:3: lv_value_0_0= RULE_INT
            {
            lv_value_0_0=(Token)match(input,RULE_INT,FOLLOW_RULE_INT_in_ruleTimeFilter1105); 

            			newLeafNode(lv_value_0_0, grammarAccess.getTimeFilterAccess().getValueINTTerminalRuleCall_0_0()); 
            		

            	        if (current==null) {
            	            current = createModelElement(grammarAccess.getTimeFilterRule());
            	        }
                   		setWithLastConsumed(
                   			current, 
                   			"value",
                    		lv_value_0_0, 
                    		"INT");
            	    

            }


            }

            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:528:2: ( ( (lv_unit_1_1= 's' | lv_unit_1_2= 'm' | lv_unit_1_3= 'h' | lv_unit_1_4= 'd' ) ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:529:1: ( (lv_unit_1_1= 's' | lv_unit_1_2= 'm' | lv_unit_1_3= 'h' | lv_unit_1_4= 'd' ) )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:529:1: ( (lv_unit_1_1= 's' | lv_unit_1_2= 'm' | lv_unit_1_3= 'h' | lv_unit_1_4= 'd' ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:530:1: (lv_unit_1_1= 's' | lv_unit_1_2= 'm' | lv_unit_1_3= 'h' | lv_unit_1_4= 'd' )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:530:1: (lv_unit_1_1= 's' | lv_unit_1_2= 'm' | lv_unit_1_3= 'h' | lv_unit_1_4= 'd' )
            int alt10=4;
            switch ( input.LA(1) ) {
            case 22:
                {
                alt10=1;
                }
                break;
            case 23:
                {
                alt10=2;
                }
                break;
            case 24:
                {
                alt10=3;
                }
                break;
            case 25:
                {
                alt10=4;
                }
                break;
            default:
                NoViableAltException nvae =
                    new NoViableAltException("", 10, 0, input);

                throw nvae;
            }

            switch (alt10) {
                case 1 :
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:531:3: lv_unit_1_1= 's'
                    {
                    lv_unit_1_1=(Token)match(input,22,FOLLOW_22_in_ruleTimeFilter1130); 

                            newLeafNode(lv_unit_1_1, grammarAccess.getTimeFilterAccess().getUnitSKeyword_1_0_0());
                        

                    	        if (current==null) {
                    	            current = createModelElement(grammarAccess.getTimeFilterRule());
                    	        }
                           		setWithLastConsumed(current, "unit", lv_unit_1_1, null);
                    	    

                    }
                    break;
                case 2 :
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:543:8: lv_unit_1_2= 'm'
                    {
                    lv_unit_1_2=(Token)match(input,23,FOLLOW_23_in_ruleTimeFilter1159); 

                            newLeafNode(lv_unit_1_2, grammarAccess.getTimeFilterAccess().getUnitMKeyword_1_0_1());
                        

                    	        if (current==null) {
                    	            current = createModelElement(grammarAccess.getTimeFilterRule());
                    	        }
                           		setWithLastConsumed(current, "unit", lv_unit_1_2, null);
                    	    

                    }
                    break;
                case 3 :
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:555:8: lv_unit_1_3= 'h'
                    {
                    lv_unit_1_3=(Token)match(input,24,FOLLOW_24_in_ruleTimeFilter1188); 

                            newLeafNode(lv_unit_1_3, grammarAccess.getTimeFilterAccess().getUnitHKeyword_1_0_2());
                        

                    	        if (current==null) {
                    	            current = createModelElement(grammarAccess.getTimeFilterRule());
                    	        }
                           		setWithLastConsumed(current, "unit", lv_unit_1_3, null);
                    	    

                    }
                    break;
                case 4 :
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:567:8: lv_unit_1_4= 'd'
                    {
                    lv_unit_1_4=(Token)match(input,25,FOLLOW_25_in_ruleTimeFilter1217); 

                            newLeafNode(lv_unit_1_4, grammarAccess.getTimeFilterAccess().getUnitDKeyword_1_0_3());
                        

                    	        if (current==null) {
                    	            current = createModelElement(grammarAccess.getTimeFilterRule());
                    	        }
                           		setWithLastConsumed(current, "unit", lv_unit_1_4, null);
                    	    

                    }
                    break;

            }


            }


            }


            }


            }

             leaveRule(); 
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "ruleTimeFilter"


    // $ANTLR start "entryRulePersistenceConfiguration"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:590:1: entryRulePersistenceConfiguration returns [EObject current=null] : iv_rulePersistenceConfiguration= rulePersistenceConfiguration EOF ;
    public final EObject entryRulePersistenceConfiguration() throws RecognitionException {
        EObject current = null;

        EObject iv_rulePersistenceConfiguration = null;


        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:591:2: (iv_rulePersistenceConfiguration= rulePersistenceConfiguration EOF )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:592:2: iv_rulePersistenceConfiguration= rulePersistenceConfiguration EOF
            {
             newCompositeNode(grammarAccess.getPersistenceConfigurationRule()); 
            pushFollow(FOLLOW_rulePersistenceConfiguration_in_entryRulePersistenceConfiguration1269);
            iv_rulePersistenceConfiguration=rulePersistenceConfiguration();

            state._fsp--;

             current =iv_rulePersistenceConfiguration; 
            match(input,EOF,FOLLOW_EOF_in_entryRulePersistenceConfiguration1279); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "entryRulePersistenceConfiguration"


    // $ANTLR start "rulePersistenceConfiguration"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:599:1: rulePersistenceConfiguration returns [EObject current=null] : ( ( ( (lv_items_0_1= ruleAllConfig | lv_items_0_2= ruleItemConfig | lv_items_0_3= ruleGroupConfig ) ) ) (otherlv_1= ',' ( ( (lv_items_2_1= ruleAllConfig | lv_items_2_2= ruleItemConfig | lv_items_2_3= ruleGroupConfig ) ) ) )* (otherlv_3= '->' ( (lv_alias_4_0= RULE_STRING ) ) )? ( (otherlv_5= ':' (otherlv_6= 'strategy' otherlv_7= '=' ( (otherlv_8= RULE_ID ) ) (otherlv_9= ',' ( (otherlv_10= RULE_ID ) ) )* )? (otherlv_11= 'filter' otherlv_12= '=' ( (otherlv_13= RULE_ID ) ) (otherlv_14= ',' ( (otherlv_15= RULE_ID ) ) )* )? ) | otherlv_16= ';' ) ) ;
    public final EObject rulePersistenceConfiguration() throws RecognitionException {
        EObject current = null;

        Token otherlv_1=null;
        Token otherlv_3=null;
        Token lv_alias_4_0=null;
        Token otherlv_5=null;
        Token otherlv_6=null;
        Token otherlv_7=null;
        Token otherlv_8=null;
        Token otherlv_9=null;
        Token otherlv_10=null;
        Token otherlv_11=null;
        Token otherlv_12=null;
        Token otherlv_13=null;
        Token otherlv_14=null;
        Token otherlv_15=null;
        Token otherlv_16=null;
        EObject lv_items_0_1 = null;

        EObject lv_items_0_2 = null;

        EObject lv_items_0_3 = null;

        EObject lv_items_2_1 = null;

        EObject lv_items_2_2 = null;

        EObject lv_items_2_3 = null;


         enterRule(); 
            
        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:602:28: ( ( ( ( (lv_items_0_1= ruleAllConfig | lv_items_0_2= ruleItemConfig | lv_items_0_3= ruleGroupConfig ) ) ) (otherlv_1= ',' ( ( (lv_items_2_1= ruleAllConfig | lv_items_2_2= ruleItemConfig | lv_items_2_3= ruleGroupConfig ) ) ) )* (otherlv_3= '->' ( (lv_alias_4_0= RULE_STRING ) ) )? ( (otherlv_5= ':' (otherlv_6= 'strategy' otherlv_7= '=' ( (otherlv_8= RULE_ID ) ) (otherlv_9= ',' ( (otherlv_10= RULE_ID ) ) )* )? (otherlv_11= 'filter' otherlv_12= '=' ( (otherlv_13= RULE_ID ) ) (otherlv_14= ',' ( (otherlv_15= RULE_ID ) ) )* )? ) | otherlv_16= ';' ) ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:603:1: ( ( ( (lv_items_0_1= ruleAllConfig | lv_items_0_2= ruleItemConfig | lv_items_0_3= ruleGroupConfig ) ) ) (otherlv_1= ',' ( ( (lv_items_2_1= ruleAllConfig | lv_items_2_2= ruleItemConfig | lv_items_2_3= ruleGroupConfig ) ) ) )* (otherlv_3= '->' ( (lv_alias_4_0= RULE_STRING ) ) )? ( (otherlv_5= ':' (otherlv_6= 'strategy' otherlv_7= '=' ( (otherlv_8= RULE_ID ) ) (otherlv_9= ',' ( (otherlv_10= RULE_ID ) ) )* )? (otherlv_11= 'filter' otherlv_12= '=' ( (otherlv_13= RULE_ID ) ) (otherlv_14= ',' ( (otherlv_15= RULE_ID ) ) )* )? ) | otherlv_16= ';' ) )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:603:1: ( ( ( (lv_items_0_1= ruleAllConfig | lv_items_0_2= ruleItemConfig | lv_items_0_3= ruleGroupConfig ) ) ) (otherlv_1= ',' ( ( (lv_items_2_1= ruleAllConfig | lv_items_2_2= ruleItemConfig | lv_items_2_3= ruleGroupConfig ) ) ) )* (otherlv_3= '->' ( (lv_alias_4_0= RULE_STRING ) ) )? ( (otherlv_5= ':' (otherlv_6= 'strategy' otherlv_7= '=' ( (otherlv_8= RULE_ID ) ) (otherlv_9= ',' ( (otherlv_10= RULE_ID ) ) )* )? (otherlv_11= 'filter' otherlv_12= '=' ( (otherlv_13= RULE_ID ) ) (otherlv_14= ',' ( (otherlv_15= RULE_ID ) ) )* )? ) | otherlv_16= ';' ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:603:2: ( ( (lv_items_0_1= ruleAllConfig | lv_items_0_2= ruleItemConfig | lv_items_0_3= ruleGroupConfig ) ) ) (otherlv_1= ',' ( ( (lv_items_2_1= ruleAllConfig | lv_items_2_2= ruleItemConfig | lv_items_2_3= ruleGroupConfig ) ) ) )* (otherlv_3= '->' ( (lv_alias_4_0= RULE_STRING ) ) )? ( (otherlv_5= ':' (otherlv_6= 'strategy' otherlv_7= '=' ( (otherlv_8= RULE_ID ) ) (otherlv_9= ',' ( (otherlv_10= RULE_ID ) ) )* )? (otherlv_11= 'filter' otherlv_12= '=' ( (otherlv_13= RULE_ID ) ) (otherlv_14= ',' ( (otherlv_15= RULE_ID ) ) )* )? ) | otherlv_16= ';' )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:603:2: ( ( (lv_items_0_1= ruleAllConfig | lv_items_0_2= ruleItemConfig | lv_items_0_3= ruleGroupConfig ) ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:604:1: ( (lv_items_0_1= ruleAllConfig | lv_items_0_2= ruleItemConfig | lv_items_0_3= ruleGroupConfig ) )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:604:1: ( (lv_items_0_1= ruleAllConfig | lv_items_0_2= ruleItemConfig | lv_items_0_3= ruleGroupConfig ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:605:1: (lv_items_0_1= ruleAllConfig | lv_items_0_2= ruleItemConfig | lv_items_0_3= ruleGroupConfig )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:605:1: (lv_items_0_1= ruleAllConfig | lv_items_0_2= ruleItemConfig | lv_items_0_3= ruleGroupConfig )
            int alt11=3;
            int LA11_0 = input.LA(1);

            if ( (LA11_0==30) ) {
                alt11=1;
            }
            else if ( (LA11_0==RULE_ID) ) {
                int LA11_2 = input.LA(2);

                if ( (LA11_2==15||LA11_2==19||LA11_2==26||LA11_2==29) ) {
                    alt11=2;
                }
                else if ( (LA11_2==30) ) {
                    alt11=3;
                }
                else {
                    NoViableAltException nvae =
                        new NoViableAltException("", 11, 2, input);

                    throw nvae;
                }
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 11, 0, input);

                throw nvae;
            }
            switch (alt11) {
                case 1 :
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:606:3: lv_items_0_1= ruleAllConfig
                    {
                     
                    	        newCompositeNode(grammarAccess.getPersistenceConfigurationAccess().getItemsAllConfigParserRuleCall_0_0_0()); 
                    	    
                    pushFollow(FOLLOW_ruleAllConfig_in_rulePersistenceConfiguration1327);
                    lv_items_0_1=ruleAllConfig();

                    state._fsp--;


                    	        if (current==null) {
                    	            current = createModelElementForParent(grammarAccess.getPersistenceConfigurationRule());
                    	        }
                           		add(
                           			current, 
                           			"items",
                            		lv_items_0_1, 
                            		"AllConfig");
                    	        afterParserOrEnumRuleCall();
                    	    

                    }
                    break;
                case 2 :
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:621:8: lv_items_0_2= ruleItemConfig
                    {
                     
                    	        newCompositeNode(grammarAccess.getPersistenceConfigurationAccess().getItemsItemConfigParserRuleCall_0_0_1()); 
                    	    
                    pushFollow(FOLLOW_ruleItemConfig_in_rulePersistenceConfiguration1346);
                    lv_items_0_2=ruleItemConfig();

                    state._fsp--;


                    	        if (current==null) {
                    	            current = createModelElementForParent(grammarAccess.getPersistenceConfigurationRule());
                    	        }
                           		add(
                           			current, 
                           			"items",
                            		lv_items_0_2, 
                            		"ItemConfig");
                    	        afterParserOrEnumRuleCall();
                    	    

                    }
                    break;
                case 3 :
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:636:8: lv_items_0_3= ruleGroupConfig
                    {
                     
                    	        newCompositeNode(grammarAccess.getPersistenceConfigurationAccess().getItemsGroupConfigParserRuleCall_0_0_2()); 
                    	    
                    pushFollow(FOLLOW_ruleGroupConfig_in_rulePersistenceConfiguration1365);
                    lv_items_0_3=ruleGroupConfig();

                    state._fsp--;


                    	        if (current==null) {
                    	            current = createModelElementForParent(grammarAccess.getPersistenceConfigurationRule());
                    	        }
                           		add(
                           			current, 
                           			"items",
                            		lv_items_0_3, 
                            		"GroupConfig");
                    	        afterParserOrEnumRuleCall();
                    	    

                    }
                    break;

            }


            }


            }

            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:654:2: (otherlv_1= ',' ( ( (lv_items_2_1= ruleAllConfig | lv_items_2_2= ruleItemConfig | lv_items_2_3= ruleGroupConfig ) ) ) )*
            loop13:
            do {
                int alt13=2;
                int LA13_0 = input.LA(1);

                if ( (LA13_0==15) ) {
                    alt13=1;
                }


                switch (alt13) {
            	case 1 :
            	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:654:4: otherlv_1= ',' ( ( (lv_items_2_1= ruleAllConfig | lv_items_2_2= ruleItemConfig | lv_items_2_3= ruleGroupConfig ) ) )
            	    {
            	    otherlv_1=(Token)match(input,15,FOLLOW_15_in_rulePersistenceConfiguration1381); 

            	        	newLeafNode(otherlv_1, grammarAccess.getPersistenceConfigurationAccess().getCommaKeyword_1_0());
            	        
            	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:658:1: ( ( (lv_items_2_1= ruleAllConfig | lv_items_2_2= ruleItemConfig | lv_items_2_3= ruleGroupConfig ) ) )
            	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:659:1: ( (lv_items_2_1= ruleAllConfig | lv_items_2_2= ruleItemConfig | lv_items_2_3= ruleGroupConfig ) )
            	    {
            	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:659:1: ( (lv_items_2_1= ruleAllConfig | lv_items_2_2= ruleItemConfig | lv_items_2_3= ruleGroupConfig ) )
            	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:660:1: (lv_items_2_1= ruleAllConfig | lv_items_2_2= ruleItemConfig | lv_items_2_3= ruleGroupConfig )
            	    {
            	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:660:1: (lv_items_2_1= ruleAllConfig | lv_items_2_2= ruleItemConfig | lv_items_2_3= ruleGroupConfig )
            	    int alt12=3;
            	    int LA12_0 = input.LA(1);

            	    if ( (LA12_0==30) ) {
            	        alt12=1;
            	    }
            	    else if ( (LA12_0==RULE_ID) ) {
            	        int LA12_2 = input.LA(2);

            	        if ( (LA12_2==30) ) {
            	            alt12=3;
            	        }
            	        else if ( (LA12_2==15||LA12_2==19||LA12_2==26||LA12_2==29) ) {
            	            alt12=2;
            	        }
            	        else {
            	            NoViableAltException nvae =
            	                new NoViableAltException("", 12, 2, input);

            	            throw nvae;
            	        }
            	    }
            	    else {
            	        NoViableAltException nvae =
            	            new NoViableAltException("", 12, 0, input);

            	        throw nvae;
            	    }
            	    switch (alt12) {
            	        case 1 :
            	            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:661:3: lv_items_2_1= ruleAllConfig
            	            {
            	             
            	            	        newCompositeNode(grammarAccess.getPersistenceConfigurationAccess().getItemsAllConfigParserRuleCall_1_1_0_0()); 
            	            	    
            	            pushFollow(FOLLOW_ruleAllConfig_in_rulePersistenceConfiguration1404);
            	            lv_items_2_1=ruleAllConfig();

            	            state._fsp--;


            	            	        if (current==null) {
            	            	            current = createModelElementForParent(grammarAccess.getPersistenceConfigurationRule());
            	            	        }
            	                   		add(
            	                   			current, 
            	                   			"items",
            	                    		lv_items_2_1, 
            	                    		"AllConfig");
            	            	        afterParserOrEnumRuleCall();
            	            	    

            	            }
            	            break;
            	        case 2 :
            	            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:676:8: lv_items_2_2= ruleItemConfig
            	            {
            	             
            	            	        newCompositeNode(grammarAccess.getPersistenceConfigurationAccess().getItemsItemConfigParserRuleCall_1_1_0_1()); 
            	            	    
            	            pushFollow(FOLLOW_ruleItemConfig_in_rulePersistenceConfiguration1423);
            	            lv_items_2_2=ruleItemConfig();

            	            state._fsp--;


            	            	        if (current==null) {
            	            	            current = createModelElementForParent(grammarAccess.getPersistenceConfigurationRule());
            	            	        }
            	                   		add(
            	                   			current, 
            	                   			"items",
            	                    		lv_items_2_2, 
            	                    		"ItemConfig");
            	            	        afterParserOrEnumRuleCall();
            	            	    

            	            }
            	            break;
            	        case 3 :
            	            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:691:8: lv_items_2_3= ruleGroupConfig
            	            {
            	             
            	            	        newCompositeNode(grammarAccess.getPersistenceConfigurationAccess().getItemsGroupConfigParserRuleCall_1_1_0_2()); 
            	            	    
            	            pushFollow(FOLLOW_ruleGroupConfig_in_rulePersistenceConfiguration1442);
            	            lv_items_2_3=ruleGroupConfig();

            	            state._fsp--;


            	            	        if (current==null) {
            	            	            current = createModelElementForParent(grammarAccess.getPersistenceConfigurationRule());
            	            	        }
            	                   		add(
            	                   			current, 
            	                   			"items",
            	                    		lv_items_2_3, 
            	                    		"GroupConfig");
            	            	        afterParserOrEnumRuleCall();
            	            	    

            	            }
            	            break;

            	    }


            	    }


            	    }


            	    }
            	    break;

            	default :
            	    break loop13;
                }
            } while (true);

            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:709:4: (otherlv_3= '->' ( (lv_alias_4_0= RULE_STRING ) ) )?
            int alt14=2;
            int LA14_0 = input.LA(1);

            if ( (LA14_0==26) ) {
                alt14=1;
            }
            switch (alt14) {
                case 1 :
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:709:6: otherlv_3= '->' ( (lv_alias_4_0= RULE_STRING ) )
                    {
                    otherlv_3=(Token)match(input,26,FOLLOW_26_in_rulePersistenceConfiguration1460); 

                        	newLeafNode(otherlv_3, grammarAccess.getPersistenceConfigurationAccess().getHyphenMinusGreaterThanSignKeyword_2_0());
                        
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:713:1: ( (lv_alias_4_0= RULE_STRING ) )
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:714:1: (lv_alias_4_0= RULE_STRING )
                    {
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:714:1: (lv_alias_4_0= RULE_STRING )
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:715:3: lv_alias_4_0= RULE_STRING
                    {
                    lv_alias_4_0=(Token)match(input,RULE_STRING,FOLLOW_RULE_STRING_in_rulePersistenceConfiguration1477); 

                    			newLeafNode(lv_alias_4_0, grammarAccess.getPersistenceConfigurationAccess().getAliasSTRINGTerminalRuleCall_2_1_0()); 
                    		

                    	        if (current==null) {
                    	            current = createModelElement(grammarAccess.getPersistenceConfigurationRule());
                    	        }
                           		setWithLastConsumed(
                           			current, 
                           			"alias",
                            		lv_alias_4_0, 
                            		"STRING");
                    	    

                    }


                    }


                    }
                    break;

            }

            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:731:4: ( (otherlv_5= ':' (otherlv_6= 'strategy' otherlv_7= '=' ( (otherlv_8= RULE_ID ) ) (otherlv_9= ',' ( (otherlv_10= RULE_ID ) ) )* )? (otherlv_11= 'filter' otherlv_12= '=' ( (otherlv_13= RULE_ID ) ) (otherlv_14= ',' ( (otherlv_15= RULE_ID ) ) )* )? ) | otherlv_16= ';' )
            int alt19=2;
            int LA19_0 = input.LA(1);

            if ( (LA19_0==19) ) {
                alt19=1;
            }
            else if ( (LA19_0==29) ) {
                alt19=2;
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 19, 0, input);

                throw nvae;
            }
            switch (alt19) {
                case 1 :
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:731:5: (otherlv_5= ':' (otherlv_6= 'strategy' otherlv_7= '=' ( (otherlv_8= RULE_ID ) ) (otherlv_9= ',' ( (otherlv_10= RULE_ID ) ) )* )? (otherlv_11= 'filter' otherlv_12= '=' ( (otherlv_13= RULE_ID ) ) (otherlv_14= ',' ( (otherlv_15= RULE_ID ) ) )* )? )
                    {
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:731:5: (otherlv_5= ':' (otherlv_6= 'strategy' otherlv_7= '=' ( (otherlv_8= RULE_ID ) ) (otherlv_9= ',' ( (otherlv_10= RULE_ID ) ) )* )? (otherlv_11= 'filter' otherlv_12= '=' ( (otherlv_13= RULE_ID ) ) (otherlv_14= ',' ( (otherlv_15= RULE_ID ) ) )* )? )
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:731:7: otherlv_5= ':' (otherlv_6= 'strategy' otherlv_7= '=' ( (otherlv_8= RULE_ID ) ) (otherlv_9= ',' ( (otherlv_10= RULE_ID ) ) )* )? (otherlv_11= 'filter' otherlv_12= '=' ( (otherlv_13= RULE_ID ) ) (otherlv_14= ',' ( (otherlv_15= RULE_ID ) ) )* )?
                    {
                    otherlv_5=(Token)match(input,19,FOLLOW_19_in_rulePersistenceConfiguration1498); 

                        	newLeafNode(otherlv_5, grammarAccess.getPersistenceConfigurationAccess().getColonKeyword_3_0_0());
                        
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:735:1: (otherlv_6= 'strategy' otherlv_7= '=' ( (otherlv_8= RULE_ID ) ) (otherlv_9= ',' ( (otherlv_10= RULE_ID ) ) )* )?
                    int alt16=2;
                    int LA16_0 = input.LA(1);

                    if ( (LA16_0==27) ) {
                        alt16=1;
                    }
                    switch (alt16) {
                        case 1 :
                            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:735:3: otherlv_6= 'strategy' otherlv_7= '=' ( (otherlv_8= RULE_ID ) ) (otherlv_9= ',' ( (otherlv_10= RULE_ID ) ) )*
                            {
                            otherlv_6=(Token)match(input,27,FOLLOW_27_in_rulePersistenceConfiguration1511); 

                                	newLeafNode(otherlv_6, grammarAccess.getPersistenceConfigurationAccess().getStrategyKeyword_3_0_1_0());
                                
                            otherlv_7=(Token)match(input,14,FOLLOW_14_in_rulePersistenceConfiguration1523); 

                                	newLeafNode(otherlv_7, grammarAccess.getPersistenceConfigurationAccess().getEqualsSignKeyword_3_0_1_1());
                                
                            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:743:1: ( (otherlv_8= RULE_ID ) )
                            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:744:1: (otherlv_8= RULE_ID )
                            {
                            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:744:1: (otherlv_8= RULE_ID )
                            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:745:3: otherlv_8= RULE_ID
                            {

                            			if (current==null) {
                            	            current = createModelElement(grammarAccess.getPersistenceConfigurationRule());
                            	        }
                                    
                            otherlv_8=(Token)match(input,RULE_ID,FOLLOW_RULE_ID_in_rulePersistenceConfiguration1543); 

                            		newLeafNode(otherlv_8, grammarAccess.getPersistenceConfigurationAccess().getStrategiesStrategyCrossReference_3_0_1_2_0()); 
                            	

                            }


                            }

                            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:756:2: (otherlv_9= ',' ( (otherlv_10= RULE_ID ) ) )*
                            loop15:
                            do {
                                int alt15=2;
                                int LA15_0 = input.LA(1);

                                if ( (LA15_0==15) ) {
                                    alt15=1;
                                }


                                switch (alt15) {
                            	case 1 :
                            	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:756:4: otherlv_9= ',' ( (otherlv_10= RULE_ID ) )
                            	    {
                            	    otherlv_9=(Token)match(input,15,FOLLOW_15_in_rulePersistenceConfiguration1556); 

                            	        	newLeafNode(otherlv_9, grammarAccess.getPersistenceConfigurationAccess().getCommaKeyword_3_0_1_3_0());
                            	        
                            	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:760:1: ( (otherlv_10= RULE_ID ) )
                            	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:761:1: (otherlv_10= RULE_ID )
                            	    {
                            	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:761:1: (otherlv_10= RULE_ID )
                            	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:762:3: otherlv_10= RULE_ID
                            	    {

                            	    			if (current==null) {
                            	    	            current = createModelElement(grammarAccess.getPersistenceConfigurationRule());
                            	    	        }
                            	            
                            	    otherlv_10=(Token)match(input,RULE_ID,FOLLOW_RULE_ID_in_rulePersistenceConfiguration1576); 

                            	    		newLeafNode(otherlv_10, grammarAccess.getPersistenceConfigurationAccess().getStrategiesStrategyCrossReference_3_0_1_3_1_0()); 
                            	    	

                            	    }


                            	    }


                            	    }
                            	    break;

                            	default :
                            	    break loop15;
                                }
                            } while (true);


                            }
                            break;

                    }

                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:773:6: (otherlv_11= 'filter' otherlv_12= '=' ( (otherlv_13= RULE_ID ) ) (otherlv_14= ',' ( (otherlv_15= RULE_ID ) ) )* )?
                    int alt18=2;
                    int LA18_0 = input.LA(1);

                    if ( (LA18_0==28) ) {
                        alt18=1;
                    }
                    switch (alt18) {
                        case 1 :
                            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:773:8: otherlv_11= 'filter' otherlv_12= '=' ( (otherlv_13= RULE_ID ) ) (otherlv_14= ',' ( (otherlv_15= RULE_ID ) ) )*
                            {
                            otherlv_11=(Token)match(input,28,FOLLOW_28_in_rulePersistenceConfiguration1593); 

                                	newLeafNode(otherlv_11, grammarAccess.getPersistenceConfigurationAccess().getFilterKeyword_3_0_2_0());
                                
                            otherlv_12=(Token)match(input,14,FOLLOW_14_in_rulePersistenceConfiguration1605); 

                                	newLeafNode(otherlv_12, grammarAccess.getPersistenceConfigurationAccess().getEqualsSignKeyword_3_0_2_1());
                                
                            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:781:1: ( (otherlv_13= RULE_ID ) )
                            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:782:1: (otherlv_13= RULE_ID )
                            {
                            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:782:1: (otherlv_13= RULE_ID )
                            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:783:3: otherlv_13= RULE_ID
                            {

                            			if (current==null) {
                            	            current = createModelElement(grammarAccess.getPersistenceConfigurationRule());
                            	        }
                                    
                            otherlv_13=(Token)match(input,RULE_ID,FOLLOW_RULE_ID_in_rulePersistenceConfiguration1625); 

                            		newLeafNode(otherlv_13, grammarAccess.getPersistenceConfigurationAccess().getFiltersFilterCrossReference_3_0_2_2_0()); 
                            	

                            }


                            }

                            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:794:2: (otherlv_14= ',' ( (otherlv_15= RULE_ID ) ) )*
                            loop17:
                            do {
                                int alt17=2;
                                int LA17_0 = input.LA(1);

                                if ( (LA17_0==15) ) {
                                    alt17=1;
                                }


                                switch (alt17) {
                            	case 1 :
                            	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:794:4: otherlv_14= ',' ( (otherlv_15= RULE_ID ) )
                            	    {
                            	    otherlv_14=(Token)match(input,15,FOLLOW_15_in_rulePersistenceConfiguration1638); 

                            	        	newLeafNode(otherlv_14, grammarAccess.getPersistenceConfigurationAccess().getCommaKeyword_3_0_2_3_0());
                            	        
                            	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:798:1: ( (otherlv_15= RULE_ID ) )
                            	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:799:1: (otherlv_15= RULE_ID )
                            	    {
                            	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:799:1: (otherlv_15= RULE_ID )
                            	    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:800:3: otherlv_15= RULE_ID
                            	    {

                            	    			if (current==null) {
                            	    	            current = createModelElement(grammarAccess.getPersistenceConfigurationRule());
                            	    	        }
                            	            
                            	    otherlv_15=(Token)match(input,RULE_ID,FOLLOW_RULE_ID_in_rulePersistenceConfiguration1658); 

                            	    		newLeafNode(otherlv_15, grammarAccess.getPersistenceConfigurationAccess().getFiltersFilterCrossReference_3_0_2_3_1_0()); 
                            	    	

                            	    }


                            	    }


                            	    }
                            	    break;

                            	default :
                            	    break loop17;
                                }
                            } while (true);


                            }
                            break;

                    }


                    }


                    }
                    break;
                case 2 :
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:812:7: otherlv_16= ';'
                    {
                    otherlv_16=(Token)match(input,29,FOLLOW_29_in_rulePersistenceConfiguration1681); 

                        	newLeafNode(otherlv_16, grammarAccess.getPersistenceConfigurationAccess().getSemicolonKeyword_3_1());
                        

                    }
                    break;

            }


            }


            }

             leaveRule(); 
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "rulePersistenceConfiguration"


    // $ANTLR start "entryRuleAllConfig"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:824:1: entryRuleAllConfig returns [EObject current=null] : iv_ruleAllConfig= ruleAllConfig EOF ;
    public final EObject entryRuleAllConfig() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleAllConfig = null;


        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:825:2: (iv_ruleAllConfig= ruleAllConfig EOF )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:826:2: iv_ruleAllConfig= ruleAllConfig EOF
            {
             newCompositeNode(grammarAccess.getAllConfigRule()); 
            pushFollow(FOLLOW_ruleAllConfig_in_entryRuleAllConfig1718);
            iv_ruleAllConfig=ruleAllConfig();

            state._fsp--;

             current =iv_ruleAllConfig; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleAllConfig1728); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "entryRuleAllConfig"


    // $ANTLR start "ruleAllConfig"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:833:1: ruleAllConfig returns [EObject current=null] : ( () otherlv_1= '*' ) ;
    public final EObject ruleAllConfig() throws RecognitionException {
        EObject current = null;

        Token otherlv_1=null;

         enterRule(); 
            
        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:836:28: ( ( () otherlv_1= '*' ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:837:1: ( () otherlv_1= '*' )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:837:1: ( () otherlv_1= '*' )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:837:2: () otherlv_1= '*'
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:837:2: ()
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:838:5: 
            {

                    current = forceCreateModelElement(
                        grammarAccess.getAllConfigAccess().getAllConfigAction_0(),
                        current);
                

            }

            otherlv_1=(Token)match(input,30,FOLLOW_30_in_ruleAllConfig1774); 

                	newLeafNode(otherlv_1, grammarAccess.getAllConfigAccess().getAsteriskKeyword_1());
                

            }


            }

             leaveRule(); 
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "ruleAllConfig"


    // $ANTLR start "entryRuleItemConfig"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:855:1: entryRuleItemConfig returns [EObject current=null] : iv_ruleItemConfig= ruleItemConfig EOF ;
    public final EObject entryRuleItemConfig() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleItemConfig = null;


        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:856:2: (iv_ruleItemConfig= ruleItemConfig EOF )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:857:2: iv_ruleItemConfig= ruleItemConfig EOF
            {
             newCompositeNode(grammarAccess.getItemConfigRule()); 
            pushFollow(FOLLOW_ruleItemConfig_in_entryRuleItemConfig1810);
            iv_ruleItemConfig=ruleItemConfig();

            state._fsp--;

             current =iv_ruleItemConfig; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleItemConfig1820); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "entryRuleItemConfig"


    // $ANTLR start "ruleItemConfig"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:864:1: ruleItemConfig returns [EObject current=null] : ( (lv_item_0_0= RULE_ID ) ) ;
    public final EObject ruleItemConfig() throws RecognitionException {
        EObject current = null;

        Token lv_item_0_0=null;

         enterRule(); 
            
        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:867:28: ( ( (lv_item_0_0= RULE_ID ) ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:868:1: ( (lv_item_0_0= RULE_ID ) )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:868:1: ( (lv_item_0_0= RULE_ID ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:869:1: (lv_item_0_0= RULE_ID )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:869:1: (lv_item_0_0= RULE_ID )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:870:3: lv_item_0_0= RULE_ID
            {
            lv_item_0_0=(Token)match(input,RULE_ID,FOLLOW_RULE_ID_in_ruleItemConfig1861); 

            			newLeafNode(lv_item_0_0, grammarAccess.getItemConfigAccess().getItemIDTerminalRuleCall_0()); 
            		

            	        if (current==null) {
            	            current = createModelElement(grammarAccess.getItemConfigRule());
            	        }
                   		setWithLastConsumed(
                   			current, 
                   			"item",
                    		lv_item_0_0, 
                    		"ID");
            	    

            }


            }


            }

             leaveRule(); 
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "ruleItemConfig"


    // $ANTLR start "entryRuleGroupConfig"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:894:1: entryRuleGroupConfig returns [EObject current=null] : iv_ruleGroupConfig= ruleGroupConfig EOF ;
    public final EObject entryRuleGroupConfig() throws RecognitionException {
        EObject current = null;

        EObject iv_ruleGroupConfig = null;


        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:895:2: (iv_ruleGroupConfig= ruleGroupConfig EOF )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:896:2: iv_ruleGroupConfig= ruleGroupConfig EOF
            {
             newCompositeNode(grammarAccess.getGroupConfigRule()); 
            pushFollow(FOLLOW_ruleGroupConfig_in_entryRuleGroupConfig1901);
            iv_ruleGroupConfig=ruleGroupConfig();

            state._fsp--;

             current =iv_ruleGroupConfig; 
            match(input,EOF,FOLLOW_EOF_in_entryRuleGroupConfig1911); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "entryRuleGroupConfig"


    // $ANTLR start "ruleGroupConfig"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:903:1: ruleGroupConfig returns [EObject current=null] : ( ( (lv_group_0_0= RULE_ID ) ) otherlv_1= '*' ) ;
    public final EObject ruleGroupConfig() throws RecognitionException {
        EObject current = null;

        Token lv_group_0_0=null;
        Token otherlv_1=null;

         enterRule(); 
            
        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:906:28: ( ( ( (lv_group_0_0= RULE_ID ) ) otherlv_1= '*' ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:907:1: ( ( (lv_group_0_0= RULE_ID ) ) otherlv_1= '*' )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:907:1: ( ( (lv_group_0_0= RULE_ID ) ) otherlv_1= '*' )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:907:2: ( (lv_group_0_0= RULE_ID ) ) otherlv_1= '*'
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:907:2: ( (lv_group_0_0= RULE_ID ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:908:1: (lv_group_0_0= RULE_ID )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:908:1: (lv_group_0_0= RULE_ID )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:909:3: lv_group_0_0= RULE_ID
            {
            lv_group_0_0=(Token)match(input,RULE_ID,FOLLOW_RULE_ID_in_ruleGroupConfig1953); 

            			newLeafNode(lv_group_0_0, grammarAccess.getGroupConfigAccess().getGroupIDTerminalRuleCall_0_0()); 
            		

            	        if (current==null) {
            	            current = createModelElement(grammarAccess.getGroupConfigRule());
            	        }
                   		setWithLastConsumed(
                   			current, 
                   			"group",
                    		lv_group_0_0, 
                    		"ID");
            	    

            }


            }

            otherlv_1=(Token)match(input,30,FOLLOW_30_in_ruleGroupConfig1970); 

                	newLeafNode(otherlv_1, grammarAccess.getGroupConfigAccess().getAsteriskKeyword_1());
                

            }


            }

             leaveRule(); 
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "ruleGroupConfig"


    // $ANTLR start "entryRuleDECIMAL"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:937:1: entryRuleDECIMAL returns [String current=null] : iv_ruleDECIMAL= ruleDECIMAL EOF ;
    public final String entryRuleDECIMAL() throws RecognitionException {
        String current = null;

        AntlrDatatypeRuleToken iv_ruleDECIMAL = null;


        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:938:2: (iv_ruleDECIMAL= ruleDECIMAL EOF )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:939:2: iv_ruleDECIMAL= ruleDECIMAL EOF
            {
             newCompositeNode(grammarAccess.getDECIMALRule()); 
            pushFollow(FOLLOW_ruleDECIMAL_in_entryRuleDECIMAL2007);
            iv_ruleDECIMAL=ruleDECIMAL();

            state._fsp--;

             current =iv_ruleDECIMAL.getText(); 
            match(input,EOF,FOLLOW_EOF_in_entryRuleDECIMAL2018); 

            }

        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "entryRuleDECIMAL"


    // $ANTLR start "ruleDECIMAL"
    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:946:1: ruleDECIMAL returns [AntlrDatatypeRuleToken current=new AntlrDatatypeRuleToken()] : (this_INT_0= RULE_INT (kw= '.' this_INT_2= RULE_INT )? ) ;
    public final AntlrDatatypeRuleToken ruleDECIMAL() throws RecognitionException {
        AntlrDatatypeRuleToken current = new AntlrDatatypeRuleToken();

        Token this_INT_0=null;
        Token kw=null;
        Token this_INT_2=null;

         enterRule(); 
            
        try {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:949:28: ( (this_INT_0= RULE_INT (kw= '.' this_INT_2= RULE_INT )? ) )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:950:1: (this_INT_0= RULE_INT (kw= '.' this_INT_2= RULE_INT )? )
            {
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:950:1: (this_INT_0= RULE_INT (kw= '.' this_INT_2= RULE_INT )? )
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:950:6: this_INT_0= RULE_INT (kw= '.' this_INT_2= RULE_INT )?
            {
            this_INT_0=(Token)match(input,RULE_INT,FOLLOW_RULE_INT_in_ruleDECIMAL2058); 

            		current.merge(this_INT_0);
                
             
                newLeafNode(this_INT_0, grammarAccess.getDECIMALAccess().getINTTerminalRuleCall_0()); 
                
            // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:957:1: (kw= '.' this_INT_2= RULE_INT )?
            int alt20=2;
            int LA20_0 = input.LA(1);

            if ( (LA20_0==31) ) {
                alt20=1;
            }
            switch (alt20) {
                case 1 :
                    // ../org.openhab.model.persistence/src-gen/org/openhab/model/persistence/parser/antlr/internal/InternalPersistence.g:958:2: kw= '.' this_INT_2= RULE_INT
                    {
                    kw=(Token)match(input,31,FOLLOW_31_in_ruleDECIMAL2077); 

                            current.merge(kw);
                            newLeafNode(kw, grammarAccess.getDECIMALAccess().getFullStopKeyword_1_0()); 
                        
                    this_INT_2=(Token)match(input,RULE_INT,FOLLOW_RULE_INT_in_ruleDECIMAL2092); 

                    		current.merge(this_INT_2);
                        
                     
                        newLeafNode(this_INT_2, grammarAccess.getDECIMALAccess().getINTTerminalRuleCall_1_1()); 
                        

                    }
                    break;

            }


            }


            }

             leaveRule(); 
        }
         
            catch (RecognitionException re) { 
                recover(input,re); 
                appendSkippedTokens();
            } 
        finally {
        }
        return current;
    }
    // $ANTLR end "ruleDECIMAL"

    // Delegated rules


 

    public static final BitSet FOLLOW_rulePersistenceModel_in_entryRulePersistenceModel75 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRulePersistenceModel85 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_11_in_rulePersistenceModel131 = new BitSet(new long[]{0x0000000000001000L});
    public static final BitSet FOLLOW_12_in_rulePersistenceModel143 = new BitSet(new long[]{0x0000000000012010L});
    public static final BitSet FOLLOW_ruleStrategy_in_rulePersistenceModel164 = new BitSet(new long[]{0x0000000000012010L});
    public static final BitSet FOLLOW_13_in_rulePersistenceModel178 = new BitSet(new long[]{0x0000000000004000L});
    public static final BitSet FOLLOW_14_in_rulePersistenceModel190 = new BitSet(new long[]{0x0000000000000010L});
    public static final BitSet FOLLOW_RULE_ID_in_rulePersistenceModel210 = new BitSet(new long[]{0x0000000000018000L});
    public static final BitSet FOLLOW_15_in_rulePersistenceModel223 = new BitSet(new long[]{0x0000000000000010L});
    public static final BitSet FOLLOW_RULE_ID_in_rulePersistenceModel243 = new BitSet(new long[]{0x0000000000018000L});
    public static final BitSet FOLLOW_16_in_rulePersistenceModel259 = new BitSet(new long[]{0x0000000000060002L});
    public static final BitSet FOLLOW_17_in_rulePersistenceModel272 = new BitSet(new long[]{0x0000000000001000L});
    public static final BitSet FOLLOW_12_in_rulePersistenceModel284 = new BitSet(new long[]{0x0000000000010010L});
    public static final BitSet FOLLOW_ruleFilter_in_rulePersistenceModel305 = new BitSet(new long[]{0x0000000000010010L});
    public static final BitSet FOLLOW_16_in_rulePersistenceModel318 = new BitSet(new long[]{0x0000000000040002L});
    public static final BitSet FOLLOW_18_in_rulePersistenceModel333 = new BitSet(new long[]{0x0000000000001000L});
    public static final BitSet FOLLOW_12_in_rulePersistenceModel345 = new BitSet(new long[]{0x0000000040010010L});
    public static final BitSet FOLLOW_rulePersistenceConfiguration_in_rulePersistenceModel366 = new BitSet(new long[]{0x0000000040010010L});
    public static final BitSet FOLLOW_16_in_rulePersistenceModel379 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleStrategy_in_entryRuleStrategy417 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleStrategy427 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleCronStrategy_in_ruleStrategy474 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_RULE_ID_in_ruleStrategy496 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleCronStrategy_in_entryRuleCronStrategy537 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleCronStrategy547 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_RULE_ID_in_ruleCronStrategy598 = new BitSet(new long[]{0x0000000000080000L});
    public static final BitSet FOLLOW_19_in_ruleCronStrategy615 = new BitSet(new long[]{0x0000000000000020L});
    public static final BitSet FOLLOW_RULE_STRING_in_ruleCronStrategy632 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleFilter_in_entryRuleFilter673 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleFilter683 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_RULE_ID_in_ruleFilter725 = new BitSet(new long[]{0x0000000000080000L});
    public static final BitSet FOLLOW_19_in_ruleFilter742 = new BitSet(new long[]{0x0000000000100040L});
    public static final BitSet FOLLOW_ruleFilterDetails_in_ruleFilter763 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleFilterDetails_in_entryRuleFilterDetails799 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleFilterDetails809 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleThresholdFilter_in_ruleFilterDetails856 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleTimeFilter_in_ruleFilterDetails883 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleThresholdFilter_in_entryRuleThresholdFilter918 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleThresholdFilter928 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_20_in_ruleThresholdFilter965 = new BitSet(new long[]{0x0000000000000040L});
    public static final BitSet FOLLOW_ruleDECIMAL_in_ruleThresholdFilter986 = new BitSet(new long[]{0x0000000000200000L});
    public static final BitSet FOLLOW_21_in_ruleThresholdFilter1004 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleTimeFilter_in_entryRuleTimeFilter1053 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleTimeFilter1063 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_RULE_INT_in_ruleTimeFilter1105 = new BitSet(new long[]{0x0000000003C00000L});
    public static final BitSet FOLLOW_22_in_ruleTimeFilter1130 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_23_in_ruleTimeFilter1159 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_24_in_ruleTimeFilter1188 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_25_in_ruleTimeFilter1217 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_rulePersistenceConfiguration_in_entryRulePersistenceConfiguration1269 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRulePersistenceConfiguration1279 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleAllConfig_in_rulePersistenceConfiguration1327 = new BitSet(new long[]{0x0000000024088000L});
    public static final BitSet FOLLOW_ruleItemConfig_in_rulePersistenceConfiguration1346 = new BitSet(new long[]{0x0000000024088000L});
    public static final BitSet FOLLOW_ruleGroupConfig_in_rulePersistenceConfiguration1365 = new BitSet(new long[]{0x0000000024088000L});
    public static final BitSet FOLLOW_15_in_rulePersistenceConfiguration1381 = new BitSet(new long[]{0x0000000040000010L});
    public static final BitSet FOLLOW_ruleAllConfig_in_rulePersistenceConfiguration1404 = new BitSet(new long[]{0x0000000024088000L});
    public static final BitSet FOLLOW_ruleItemConfig_in_rulePersistenceConfiguration1423 = new BitSet(new long[]{0x0000000024088000L});
    public static final BitSet FOLLOW_ruleGroupConfig_in_rulePersistenceConfiguration1442 = new BitSet(new long[]{0x0000000024088000L});
    public static final BitSet FOLLOW_26_in_rulePersistenceConfiguration1460 = new BitSet(new long[]{0x0000000000000020L});
    public static final BitSet FOLLOW_RULE_STRING_in_rulePersistenceConfiguration1477 = new BitSet(new long[]{0x0000000020080000L});
    public static final BitSet FOLLOW_19_in_rulePersistenceConfiguration1498 = new BitSet(new long[]{0x0000000018000002L});
    public static final BitSet FOLLOW_27_in_rulePersistenceConfiguration1511 = new BitSet(new long[]{0x0000000000004000L});
    public static final BitSet FOLLOW_14_in_rulePersistenceConfiguration1523 = new BitSet(new long[]{0x0000000000000010L});
    public static final BitSet FOLLOW_RULE_ID_in_rulePersistenceConfiguration1543 = new BitSet(new long[]{0x0000000010008002L});
    public static final BitSet FOLLOW_15_in_rulePersistenceConfiguration1556 = new BitSet(new long[]{0x0000000000000010L});
    public static final BitSet FOLLOW_RULE_ID_in_rulePersistenceConfiguration1576 = new BitSet(new long[]{0x0000000010008002L});
    public static final BitSet FOLLOW_28_in_rulePersistenceConfiguration1593 = new BitSet(new long[]{0x0000000000004000L});
    public static final BitSet FOLLOW_14_in_rulePersistenceConfiguration1605 = new BitSet(new long[]{0x0000000000000010L});
    public static final BitSet FOLLOW_RULE_ID_in_rulePersistenceConfiguration1625 = new BitSet(new long[]{0x0000000000008002L});
    public static final BitSet FOLLOW_15_in_rulePersistenceConfiguration1638 = new BitSet(new long[]{0x0000000000000010L});
    public static final BitSet FOLLOW_RULE_ID_in_rulePersistenceConfiguration1658 = new BitSet(new long[]{0x0000000000008002L});
    public static final BitSet FOLLOW_29_in_rulePersistenceConfiguration1681 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleAllConfig_in_entryRuleAllConfig1718 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleAllConfig1728 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_30_in_ruleAllConfig1774 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleItemConfig_in_entryRuleItemConfig1810 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleItemConfig1820 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_RULE_ID_in_ruleItemConfig1861 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleGroupConfig_in_entryRuleGroupConfig1901 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleGroupConfig1911 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_RULE_ID_in_ruleGroupConfig1953 = new BitSet(new long[]{0x0000000040000000L});
    public static final BitSet FOLLOW_30_in_ruleGroupConfig1970 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_ruleDECIMAL_in_entryRuleDECIMAL2007 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_entryRuleDECIMAL2018 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_RULE_INT_in_ruleDECIMAL2058 = new BitSet(new long[]{0x0000000080000002L});
    public static final BitSet FOLLOW_31_in_ruleDECIMAL2077 = new BitSet(new long[]{0x0000000000000040L});
    public static final BitSet FOLLOW_RULE_INT_in_ruleDECIMAL2092 = new BitSet(new long[]{0x0000000000000002L});

}