/**
 */
package org.openhab.model.persistence.persistence;

import org.eclipse.emf.ecore.EFactory;

/**
 * <!-- begin-user-doc -->
 * The <b>Factory</b> for the model.
 * It provides a create method for each non-abstract class of the model.
 * <!-- end-user-doc -->
 * @see org.openhab.model.persistence.persistence.PersistencePackage
 * @generated
 */
public interface PersistenceFactory extends EFactory
{
  /**
   * The singleton instance of the factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  PersistenceFactory eINSTANCE = org.openhab.model.persistence.persistence.impl.PersistenceFactoryImpl.init();

  /**
   * Returns a new object of class '<em>Model</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>Model</em>'.
   * @generated
   */
  PersistenceModel createPersistenceModel();

  /**
   * Returns a new object of class '<em>Strategy</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>Strategy</em>'.
   * @generated
   */
  Strategy createStrategy();

  /**
   * Returns a new object of class '<em>Filter</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>Filter</em>'.
   * @generated
   */
  Filter createFilter();

  /**
   * Returns a new object of class '<em>Filter Details</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>Filter Details</em>'.
   * @generated
   */
  FilterDetails createFilterDetails();

  /**
   * Returns a new object of class '<em>Threshold Filter</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>Threshold Filter</em>'.
   * @generated
   */
  ThresholdFilter createThresholdFilter();

  /**
   * Returns a new object of class '<em>Time Filter</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>Time Filter</em>'.
   * @generated
   */
  TimeFilter createTimeFilter();

  /**
   * Returns a new object of class '<em>Configuration</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>Configuration</em>'.
   * @generated
   */
  PersistenceConfiguration createPersistenceConfiguration();

  /**
   * Returns a new object of class '<em>All Config</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>All Config</em>'.
   * @generated
   */
  AllConfig createAllConfig();

  /**
   * Returns a new object of class '<em>Item Config</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>Item Config</em>'.
   * @generated
   */
  ItemConfig createItemConfig();

  /**
   * Returns a new object of class '<em>Group Config</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>Group Config</em>'.
   * @generated
   */
  GroupConfig createGroupConfig();

  /**
   * Returns a new object of class '<em>Cron Strategy</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return a new object of class '<em>Cron Strategy</em>'.
   * @generated
   */
  CronStrategy createCronStrategy();

  /**
   * Returns the package supported by this factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the package supported by this factory.
   * @generated
   */
  PersistencePackage getPersistencePackage();

} //PersistenceFactory
