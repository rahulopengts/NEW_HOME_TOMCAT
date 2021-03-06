/**
 */
package org.openhab.model.persistence.persistence;


/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Time Filter</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.openhab.model.persistence.persistence.TimeFilter#getValue <em>Value</em>}</li>
 *   <li>{@link org.openhab.model.persistence.persistence.TimeFilter#getUnit <em>Unit</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.openhab.model.persistence.persistence.PersistencePackage#getTimeFilter()
 * @model
 * @generated
 */
public interface TimeFilter extends FilterDetails
{
  /**
   * Returns the value of the '<em><b>Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Value</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Value</em>' attribute.
   * @see #setValue(int)
   * @see org.openhab.model.persistence.persistence.PersistencePackage#getTimeFilter_Value()
   * @model
   * @generated
   */
  int getValue();

  /**
   * Sets the value of the '{@link org.openhab.model.persistence.persistence.TimeFilter#getValue <em>Value</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Value</em>' attribute.
   * @see #getValue()
   * @generated
   */
  void setValue(int value);

  /**
   * Returns the value of the '<em><b>Unit</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Unit</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Unit</em>' attribute.
   * @see #setUnit(String)
   * @see org.openhab.model.persistence.persistence.PersistencePackage#getTimeFilter_Unit()
   * @model
   * @generated
   */
  String getUnit();

  /**
   * Sets the value of the '{@link org.openhab.model.persistence.persistence.TimeFilter#getUnit <em>Unit</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Unit</em>' attribute.
   * @see #getUnit()
   * @generated
   */
  void setUnit(String value);

} // TimeFilter
