/**
 */
package org.openhab.model.persistence.persistence;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Filter</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.openhab.model.persistence.persistence.Filter#getName <em>Name</em>}</li>
 *   <li>{@link org.openhab.model.persistence.persistence.Filter#getDefinition <em>Definition</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.openhab.model.persistence.persistence.PersistencePackage#getFilter()
 * @model
 * @generated
 */
public interface Filter extends EObject
{
  /**
   * Returns the value of the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Name</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Name</em>' attribute.
   * @see #setName(String)
   * @see org.openhab.model.persistence.persistence.PersistencePackage#getFilter_Name()
   * @model
   * @generated
   */
  String getName();

  /**
   * Sets the value of the '{@link org.openhab.model.persistence.persistence.Filter#getName <em>Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Name</em>' attribute.
   * @see #getName()
   * @generated
   */
  void setName(String value);

  /**
   * Returns the value of the '<em><b>Definition</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Definition</em>' containment reference isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Definition</em>' containment reference.
   * @see #setDefinition(FilterDetails)
   * @see org.openhab.model.persistence.persistence.PersistencePackage#getFilter_Definition()
   * @model containment="true"
   * @generated
   */
  FilterDetails getDefinition();

  /**
   * Sets the value of the '{@link org.openhab.model.persistence.persistence.Filter#getDefinition <em>Definition</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Definition</em>' containment reference.
   * @see #getDefinition()
   * @generated
   */
  void setDefinition(FilterDetails value);

} // Filter
