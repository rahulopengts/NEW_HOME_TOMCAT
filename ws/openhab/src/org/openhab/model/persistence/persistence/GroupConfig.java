/**
 */
package org.openhab.model.persistence.persistence;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Group Config</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.openhab.model.persistence.persistence.GroupConfig#getGroup <em>Group</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.openhab.model.persistence.persistence.PersistencePackage#getGroupConfig()
 * @model
 * @generated
 */
public interface GroupConfig extends EObject
{
  /**
   * Returns the value of the '<em><b>Group</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Group</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Group</em>' attribute.
   * @see #setGroup(String)
   * @see org.openhab.model.persistence.persistence.PersistencePackage#getGroupConfig_Group()
   * @model
   * @generated
   */
  String getGroup();

  /**
   * Sets the value of the '{@link org.openhab.model.persistence.persistence.GroupConfig#getGroup <em>Group</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Group</em>' attribute.
   * @see #getGroup()
   * @generated
   */
  void setGroup(String value);

} // GroupConfig
