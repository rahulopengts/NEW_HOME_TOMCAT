/**
 */
package org.openhab.model.persistence.persistence;


/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Cron Strategy</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.openhab.model.persistence.persistence.CronStrategy#getCronExpression <em>Cron Expression</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.openhab.model.persistence.persistence.PersistencePackage#getCronStrategy()
 * @model
 * @generated
 */
public interface CronStrategy extends Strategy
{
  /**
   * Returns the value of the '<em><b>Cron Expression</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Cron Expression</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Cron Expression</em>' attribute.
   * @see #setCronExpression(String)
   * @see org.openhab.model.persistence.persistence.PersistencePackage#getCronStrategy_CronExpression()
   * @model
   * @generated
   */
  String getCronExpression();

  /**
   * Sets the value of the '{@link org.openhab.model.persistence.persistence.CronStrategy#getCronExpression <em>Cron Expression</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Cron Expression</em>' attribute.
   * @see #getCronExpression()
   * @generated
   */
  void setCronExpression(String value);

} // CronStrategy
