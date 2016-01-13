/**
 */
package org.openhab.model.rule.rules.util;

import org.eclipse.emf.common.notify.Adapter;
import org.eclipse.emf.common.notify.Notifier;

import org.eclipse.emf.common.notify.impl.AdapterFactoryImpl;

import org.eclipse.emf.ecore.EObject;

import org.openhab.model.rule.rules.*;

/**
 * <!-- begin-user-doc -->
 * The <b>Adapter Factory</b> for the model.
 * It provides an adapter <code>createXXX</code> method for each class of the model.
 * <!-- end-user-doc -->
 * @see org.openhab.model.rule.rules.RulesPackage
 * @generated
 */
public class RulesAdapterFactory extends AdapterFactoryImpl
{
  /**
   * The cached model package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  protected static RulesPackage modelPackage;

  /**
   * Creates an instance of the adapter factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public RulesAdapterFactory()
  {
    if (modelPackage == null)
    {
      modelPackage = RulesPackage.eINSTANCE;
    }
  }

  /**
   * Returns whether this factory is applicable for the type of the object.
   * <!-- begin-user-doc -->
   * This implementation returns <code>true</code> if the object is either the model's package or is an instance object of the model.
   * <!-- end-user-doc -->
   * @return whether this factory is applicable for the type of the object.
   * @generated
   */
  @Override
  public boolean isFactoryForType(Object object)
  {
    if (object == modelPackage)
    {
      return true;
    }
    if (object instanceof EObject)
    {
      return ((EObject)object).eClass().getEPackage() == modelPackage;
    }
    return false;
  }

  /**
   * The switch that delegates to the <code>createXXX</code> methods.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  protected RulesSwitch<Adapter> modelSwitch =
    new RulesSwitch<Adapter>()
    {
      @Override
      public Adapter caseRuleModel(RuleModel object)
      {
        return createRuleModelAdapter();
      }
      @Override
      public Adapter caseImport(Import object)
      {
        return createImportAdapter();
      }
      @Override
      public Adapter caseRule(Rule object)
      {
        return createRuleAdapter();
      }
      @Override
      public Adapter caseEventTrigger(EventTrigger object)
      {
        return createEventTriggerAdapter();
      }
      @Override
      public Adapter caseCommandEventTrigger(CommandEventTrigger object)
      {
        return createCommandEventTriggerAdapter();
      }
      @Override
      public Adapter caseUpdateEventTrigger(UpdateEventTrigger object)
      {
        return createUpdateEventTriggerAdapter();
      }
      @Override
      public Adapter caseChangedEventTrigger(ChangedEventTrigger object)
      {
        return createChangedEventTriggerAdapter();
      }
      @Override
      public Adapter caseTimerTrigger(TimerTrigger object)
      {
        return createTimerTriggerAdapter();
      }
      @Override
      public Adapter caseSystemTrigger(SystemTrigger object)
      {
        return createSystemTriggerAdapter();
      }
      @Override
      public Adapter caseSystemOnStartupTrigger(SystemOnStartupTrigger object)
      {
        return createSystemOnStartupTriggerAdapter();
      }
      @Override
      public Adapter caseSystemOnShutdownTrigger(SystemOnShutdownTrigger object)
      {
        return createSystemOnShutdownTriggerAdapter();
      }
      @Override
      public Adapter defaultCase(EObject object)
      {
        return createEObjectAdapter();
      }
    };

  /**
   * Creates an adapter for the <code>target</code>.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param target the object to adapt.
   * @return the adapter for the <code>target</code>.
   * @generated
   */
  @Override
  public Adapter createAdapter(Notifier target)
  {
    return modelSwitch.doSwitch((EObject)target);
  }


  /**
   * Creates a new adapter for an object of class '{@link org.openhab.model.rule.rules.RuleModel <em>Rule Model</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   * @return the new adapter.
   * @see org.openhab.model.rule.rules.RuleModel
   * @generated
   */
  public Adapter createRuleModelAdapter()
  {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link org.openhab.model.rule.rules.Import <em>Import</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   * @return the new adapter.
   * @see org.openhab.model.rule.rules.Import
   * @generated
   */
  public Adapter createImportAdapter()
  {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link org.openhab.model.rule.rules.Rule <em>Rule</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   * @return the new adapter.
   * @see org.openhab.model.rule.rules.Rule
   * @generated
   */
  public Adapter createRuleAdapter()
  {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link org.openhab.model.rule.rules.EventTrigger <em>Event Trigger</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   * @return the new adapter.
   * @see org.openhab.model.rule.rules.EventTrigger
   * @generated
   */
  public Adapter createEventTriggerAdapter()
  {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link org.openhab.model.rule.rules.CommandEventTrigger <em>Command Event Trigger</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   * @return the new adapter.
   * @see org.openhab.model.rule.rules.CommandEventTrigger
   * @generated
   */
  public Adapter createCommandEventTriggerAdapter()
  {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link org.openhab.model.rule.rules.UpdateEventTrigger <em>Update Event Trigger</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   * @return the new adapter.
   * @see org.openhab.model.rule.rules.UpdateEventTrigger
   * @generated
   */
  public Adapter createUpdateEventTriggerAdapter()
  {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link org.openhab.model.rule.rules.ChangedEventTrigger <em>Changed Event Trigger</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   * @return the new adapter.
   * @see org.openhab.model.rule.rules.ChangedEventTrigger
   * @generated
   */
  public Adapter createChangedEventTriggerAdapter()
  {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link org.openhab.model.rule.rules.TimerTrigger <em>Timer Trigger</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   * @return the new adapter.
   * @see org.openhab.model.rule.rules.TimerTrigger
   * @generated
   */
  public Adapter createTimerTriggerAdapter()
  {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link org.openhab.model.rule.rules.SystemTrigger <em>System Trigger</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   * @return the new adapter.
   * @see org.openhab.model.rule.rules.SystemTrigger
   * @generated
   */
  public Adapter createSystemTriggerAdapter()
  {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link org.openhab.model.rule.rules.SystemOnStartupTrigger <em>System On Startup Trigger</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   * @return the new adapter.
   * @see org.openhab.model.rule.rules.SystemOnStartupTrigger
   * @generated
   */
  public Adapter createSystemOnStartupTriggerAdapter()
  {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link org.openhab.model.rule.rules.SystemOnShutdownTrigger <em>System On Shutdown Trigger</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   * @return the new adapter.
   * @see org.openhab.model.rule.rules.SystemOnShutdownTrigger
   * @generated
   */
  public Adapter createSystemOnShutdownTriggerAdapter()
  {
    return null;
  }

  /**
   * Creates a new adapter for the default case.
   * <!-- begin-user-doc -->
   * This default implementation returns null.
   * <!-- end-user-doc -->
   * @return the new adapter.
   * @generated
   */
  public Adapter createEObjectAdapter()
  {
    return null;
  }

} //RulesAdapterFactory
