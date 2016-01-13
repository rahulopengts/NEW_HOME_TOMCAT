/**
 */
package org.openhab.model.persistence.persistence;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Model</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.openhab.model.persistence.persistence.PersistenceModel#getStrategies <em>Strategies</em>}</li>
 *   <li>{@link org.openhab.model.persistence.persistence.PersistenceModel#getDefaults <em>Defaults</em>}</li>
 *   <li>{@link org.openhab.model.persistence.persistence.PersistenceModel#getFilters <em>Filters</em>}</li>
 *   <li>{@link org.openhab.model.persistence.persistence.PersistenceModel#getConfigs <em>Configs</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.openhab.model.persistence.persistence.PersistencePackage#getPersistenceModel()
 * @model
 * @generated
 */
public interface PersistenceModel extends EObject
{
  /**
   * Returns the value of the '<em><b>Strategies</b></em>' containment reference list.
   * The list contents are of type {@link org.openhab.model.persistence.persistence.Strategy}.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Strategies</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Strategies</em>' containment reference list.
   * @see org.openhab.model.persistence.persistence.PersistencePackage#getPersistenceModel_Strategies()
   * @model containment="true"
   * @generated
   */
  EList<Strategy> getStrategies();

  /**
   * Returns the value of the '<em><b>Defaults</b></em>' reference list.
   * The list contents are of type {@link org.openhab.model.persistence.persistence.Strategy}.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Defaults</em>' reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Defaults</em>' reference list.
   * @see org.openhab.model.persistence.persistence.PersistencePackage#getPersistenceModel_Defaults()
   * @model
   * @generated
   */
  EList<Strategy> getDefaults();

  /**
   * Returns the value of the '<em><b>Filters</b></em>' containment reference list.
   * The list contents are of type {@link org.openhab.model.persistence.persistence.Filter}.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Filters</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Filters</em>' containment reference list.
   * @see org.openhab.model.persistence.persistence.PersistencePackage#getPersistenceModel_Filters()
   * @model containment="true"
   * @generated
   */
  EList<Filter> getFilters();

  /**
   * Returns the value of the '<em><b>Configs</b></em>' containment reference list.
   * The list contents are of type {@link org.openhab.model.persistence.persistence.PersistenceConfiguration}.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Configs</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Configs</em>' containment reference list.
   * @see org.openhab.model.persistence.persistence.PersistencePackage#getPersistenceModel_Configs()
   * @model containment="true"
   * @generated
   */
  EList<PersistenceConfiguration> getConfigs();

} // PersistenceModel
