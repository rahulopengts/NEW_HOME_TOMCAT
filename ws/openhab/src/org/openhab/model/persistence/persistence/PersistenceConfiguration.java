/**
 */
package org.openhab.model.persistence.persistence;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Configuration</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.openhab.model.persistence.persistence.PersistenceConfiguration#getItems <em>Items</em>}</li>
 *   <li>{@link org.openhab.model.persistence.persistence.PersistenceConfiguration#getAlias <em>Alias</em>}</li>
 *   <li>{@link org.openhab.model.persistence.persistence.PersistenceConfiguration#getStrategies <em>Strategies</em>}</li>
 *   <li>{@link org.openhab.model.persistence.persistence.PersistenceConfiguration#getFilters <em>Filters</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.openhab.model.persistence.persistence.PersistencePackage#getPersistenceConfiguration()
 * @model
 * @generated
 */
public interface PersistenceConfiguration extends EObject
{
  /**
   * Returns the value of the '<em><b>Items</b></em>' containment reference list.
   * The list contents are of type {@link org.eclipse.emf.ecore.EObject}.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Items</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Items</em>' containment reference list.
   * @see org.openhab.model.persistence.persistence.PersistencePackage#getPersistenceConfiguration_Items()
   * @model containment="true"
   * @generated
   */
  EList<EObject> getItems();

  /**
   * Returns the value of the '<em><b>Alias</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Alias</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Alias</em>' attribute.
   * @see #setAlias(String)
   * @see org.openhab.model.persistence.persistence.PersistencePackage#getPersistenceConfiguration_Alias()
   * @model
   * @generated
   */
  String getAlias();

  /**
   * Sets the value of the '{@link org.openhab.model.persistence.persistence.PersistenceConfiguration#getAlias <em>Alias</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Alias</em>' attribute.
   * @see #getAlias()
   * @generated
   */
  void setAlias(String value);

  /**
   * Returns the value of the '<em><b>Strategies</b></em>' reference list.
   * The list contents are of type {@link org.openhab.model.persistence.persistence.Strategy}.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Strategies</em>' reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Strategies</em>' reference list.
   * @see org.openhab.model.persistence.persistence.PersistencePackage#getPersistenceConfiguration_Strategies()
   * @model
   * @generated
   */
  EList<Strategy> getStrategies();

  /**
   * Returns the value of the '<em><b>Filters</b></em>' reference list.
   * The list contents are of type {@link org.openhab.model.persistence.persistence.Filter}.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Filters</em>' reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Filters</em>' reference list.
   * @see org.openhab.model.persistence.persistence.PersistencePackage#getPersistenceConfiguration_Filters()
   * @model
   * @generated
   */
  EList<Filter> getFilters();

} // PersistenceConfiguration
