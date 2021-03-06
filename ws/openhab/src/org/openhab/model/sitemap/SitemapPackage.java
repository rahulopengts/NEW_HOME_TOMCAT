/**
 */
package org.openhab.model.sitemap;

import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.EReference;

/**
 * <!-- begin-user-doc -->
 * The <b>Package</b> for the model.
 * It contains accessors for the meta objects to represent
 * <ul>
 *   <li>each class,</li>
 *   <li>each feature of each class,</li>
 *   <li>each enum,</li>
 *   <li>and each data type</li>
 * </ul>
 * <!-- end-user-doc -->
 * @see org.openhab.model.sitemap.SitemapFactory
 * @model kind="package"
 * @generated
 */
public interface SitemapPackage extends EPackage
{
  /**
   * The package name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  String eNAME = "sitemap";

  /**
   * The package namespace URI.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  String eNS_URI = "http://www.openhab.org/model/Sitemap";

  /**
   * The package namespace name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  String eNS_PREFIX = "sitemap";

  /**
   * The singleton instance of the package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  SitemapPackage eINSTANCE = org.openhab.model.sitemap.impl.SitemapPackageImpl.init();

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.SitemapModelImpl <em>Model</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.SitemapModelImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getSitemapModel()
   * @generated
   */
  int SITEMAP_MODEL = 0;

  /**
   * The number of structural features of the '<em>Model</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SITEMAP_MODEL_FEATURE_COUNT = 0;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.SitemapImpl <em>Sitemap</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.SitemapImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getSitemap()
   * @generated
   */
  int SITEMAP = 1;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SITEMAP__NAME = SITEMAP_MODEL_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Label</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SITEMAP__LABEL = SITEMAP_MODEL_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Icon</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SITEMAP__ICON = SITEMAP_MODEL_FEATURE_COUNT + 2;

  /**
   * The feature id for the '<em><b>Children</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SITEMAP__CHILDREN = SITEMAP_MODEL_FEATURE_COUNT + 3;

  /**
   * The number of structural features of the '<em>Sitemap</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SITEMAP_FEATURE_COUNT = SITEMAP_MODEL_FEATURE_COUNT + 4;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.WidgetImpl <em>Widget</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.WidgetImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getWidget()
   * @generated
   */
  int WIDGET = 2;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int WIDGET__ITEM = 0;

  /**
   * The feature id for the '<em><b>Label</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int WIDGET__LABEL = 1;

  /**
   * The feature id for the '<em><b>Icon</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int WIDGET__ICON = 2;

  /**
   * The feature id for the '<em><b>Label Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int WIDGET__LABEL_COLOR = 3;

  /**
   * The feature id for the '<em><b>Value Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int WIDGET__VALUE_COLOR = 4;

  /**
   * The feature id for the '<em><b>Visibility</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int WIDGET__VISIBILITY = 5;

  /**
   * The number of structural features of the '<em>Widget</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int WIDGET_FEATURE_COUNT = 6;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.NonLinkableWidgetImpl <em>Non Linkable Widget</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.NonLinkableWidgetImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getNonLinkableWidget()
   * @generated
   */
  int NON_LINKABLE_WIDGET = 3;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int NON_LINKABLE_WIDGET__ITEM = WIDGET__ITEM;

  /**
   * The feature id for the '<em><b>Label</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int NON_LINKABLE_WIDGET__LABEL = WIDGET__LABEL;

  /**
   * The feature id for the '<em><b>Icon</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int NON_LINKABLE_WIDGET__ICON = WIDGET__ICON;

  /**
   * The feature id for the '<em><b>Label Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int NON_LINKABLE_WIDGET__LABEL_COLOR = WIDGET__LABEL_COLOR;

  /**
   * The feature id for the '<em><b>Value Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int NON_LINKABLE_WIDGET__VALUE_COLOR = WIDGET__VALUE_COLOR;

  /**
   * The feature id for the '<em><b>Visibility</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int NON_LINKABLE_WIDGET__VISIBILITY = WIDGET__VISIBILITY;

  /**
   * The number of structural features of the '<em>Non Linkable Widget</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int NON_LINKABLE_WIDGET_FEATURE_COUNT = WIDGET_FEATURE_COUNT + 0;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.LinkableWidgetImpl <em>Linkable Widget</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.LinkableWidgetImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getLinkableWidget()
   * @generated
   */
  int LINKABLE_WIDGET = 4;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int LINKABLE_WIDGET__ITEM = WIDGET__ITEM;

  /**
   * The feature id for the '<em><b>Label</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int LINKABLE_WIDGET__LABEL = WIDGET__LABEL;

  /**
   * The feature id for the '<em><b>Icon</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int LINKABLE_WIDGET__ICON = WIDGET__ICON;

  /**
   * The feature id for the '<em><b>Label Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int LINKABLE_WIDGET__LABEL_COLOR = WIDGET__LABEL_COLOR;

  /**
   * The feature id for the '<em><b>Value Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int LINKABLE_WIDGET__VALUE_COLOR = WIDGET__VALUE_COLOR;

  /**
   * The feature id for the '<em><b>Visibility</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int LINKABLE_WIDGET__VISIBILITY = WIDGET__VISIBILITY;

  /**
   * The feature id for the '<em><b>Children</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int LINKABLE_WIDGET__CHILDREN = WIDGET_FEATURE_COUNT + 0;

  /**
   * The number of structural features of the '<em>Linkable Widget</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int LINKABLE_WIDGET_FEATURE_COUNT = WIDGET_FEATURE_COUNT + 1;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.FrameImpl <em>Frame</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.FrameImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getFrame()
   * @generated
   */
  int FRAME = 5;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int FRAME__ITEM = LINKABLE_WIDGET__ITEM;

  /**
   * The feature id for the '<em><b>Label</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int FRAME__LABEL = LINKABLE_WIDGET__LABEL;

  /**
   * The feature id for the '<em><b>Icon</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int FRAME__ICON = LINKABLE_WIDGET__ICON;

  /**
   * The feature id for the '<em><b>Label Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int FRAME__LABEL_COLOR = LINKABLE_WIDGET__LABEL_COLOR;

  /**
   * The feature id for the '<em><b>Value Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int FRAME__VALUE_COLOR = LINKABLE_WIDGET__VALUE_COLOR;

  /**
   * The feature id for the '<em><b>Visibility</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int FRAME__VISIBILITY = LINKABLE_WIDGET__VISIBILITY;

  /**
   * The feature id for the '<em><b>Children</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int FRAME__CHILDREN = LINKABLE_WIDGET__CHILDREN;

  /**
   * The number of structural features of the '<em>Frame</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int FRAME_FEATURE_COUNT = LINKABLE_WIDGET_FEATURE_COUNT + 0;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.TextImpl <em>Text</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.TextImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getText()
   * @generated
   */
  int TEXT = 6;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int TEXT__ITEM = LINKABLE_WIDGET__ITEM;

  /**
   * The feature id for the '<em><b>Label</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int TEXT__LABEL = LINKABLE_WIDGET__LABEL;

  /**
   * The feature id for the '<em><b>Icon</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int TEXT__ICON = LINKABLE_WIDGET__ICON;

  /**
   * The feature id for the '<em><b>Label Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int TEXT__LABEL_COLOR = LINKABLE_WIDGET__LABEL_COLOR;

  /**
   * The feature id for the '<em><b>Value Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int TEXT__VALUE_COLOR = LINKABLE_WIDGET__VALUE_COLOR;

  /**
   * The feature id for the '<em><b>Visibility</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int TEXT__VISIBILITY = LINKABLE_WIDGET__VISIBILITY;

  /**
   * The feature id for the '<em><b>Children</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int TEXT__CHILDREN = LINKABLE_WIDGET__CHILDREN;

  /**
   * The number of structural features of the '<em>Text</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int TEXT_FEATURE_COUNT = LINKABLE_WIDGET_FEATURE_COUNT + 0;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.GroupImpl <em>Group</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.GroupImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getGroup()
   * @generated
   */
  int GROUP = 7;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int GROUP__ITEM = LINKABLE_WIDGET__ITEM;

  /**
   * The feature id for the '<em><b>Label</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int GROUP__LABEL = LINKABLE_WIDGET__LABEL;

  /**
   * The feature id for the '<em><b>Icon</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int GROUP__ICON = LINKABLE_WIDGET__ICON;

  /**
   * The feature id for the '<em><b>Label Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int GROUP__LABEL_COLOR = LINKABLE_WIDGET__LABEL_COLOR;

  /**
   * The feature id for the '<em><b>Value Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int GROUP__VALUE_COLOR = LINKABLE_WIDGET__VALUE_COLOR;

  /**
   * The feature id for the '<em><b>Visibility</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int GROUP__VISIBILITY = LINKABLE_WIDGET__VISIBILITY;

  /**
   * The feature id for the '<em><b>Children</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int GROUP__CHILDREN = LINKABLE_WIDGET__CHILDREN;

  /**
   * The number of structural features of the '<em>Group</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int GROUP_FEATURE_COUNT = LINKABLE_WIDGET_FEATURE_COUNT + 0;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.ImageImpl <em>Image</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.ImageImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getImage()
   * @generated
   */
  int IMAGE = 8;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int IMAGE__ITEM = LINKABLE_WIDGET__ITEM;

  /**
   * The feature id for the '<em><b>Label</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int IMAGE__LABEL = LINKABLE_WIDGET__LABEL;

  /**
   * The feature id for the '<em><b>Icon</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int IMAGE__ICON = LINKABLE_WIDGET__ICON;

  /**
   * The feature id for the '<em><b>Label Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int IMAGE__LABEL_COLOR = LINKABLE_WIDGET__LABEL_COLOR;

  /**
   * The feature id for the '<em><b>Value Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int IMAGE__VALUE_COLOR = LINKABLE_WIDGET__VALUE_COLOR;

  /**
   * The feature id for the '<em><b>Visibility</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int IMAGE__VISIBILITY = LINKABLE_WIDGET__VISIBILITY;

  /**
   * The feature id for the '<em><b>Children</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int IMAGE__CHILDREN = LINKABLE_WIDGET__CHILDREN;

  /**
   * The feature id for the '<em><b>Url</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int IMAGE__URL = LINKABLE_WIDGET_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Refresh</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int IMAGE__REFRESH = LINKABLE_WIDGET_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Icon Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int IMAGE__ICON_COLOR = LINKABLE_WIDGET_FEATURE_COUNT + 2;

  /**
   * The number of structural features of the '<em>Image</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int IMAGE_FEATURE_COUNT = LINKABLE_WIDGET_FEATURE_COUNT + 3;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.VideoImpl <em>Video</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.VideoImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getVideo()
   * @generated
   */
  int VIDEO = 9;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int VIDEO__ITEM = NON_LINKABLE_WIDGET__ITEM;

  /**
   * The feature id for the '<em><b>Label</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int VIDEO__LABEL = NON_LINKABLE_WIDGET__LABEL;

  /**
   * The feature id for the '<em><b>Icon</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int VIDEO__ICON = NON_LINKABLE_WIDGET__ICON;

  /**
   * The feature id for the '<em><b>Label Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int VIDEO__LABEL_COLOR = NON_LINKABLE_WIDGET__LABEL_COLOR;

  /**
   * The feature id for the '<em><b>Value Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int VIDEO__VALUE_COLOR = NON_LINKABLE_WIDGET__VALUE_COLOR;

  /**
   * The feature id for the '<em><b>Visibility</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int VIDEO__VISIBILITY = NON_LINKABLE_WIDGET__VISIBILITY;

  /**
   * The feature id for the '<em><b>Url</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int VIDEO__URL = NON_LINKABLE_WIDGET_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Encoding</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int VIDEO__ENCODING = NON_LINKABLE_WIDGET_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Video</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int VIDEO_FEATURE_COUNT = NON_LINKABLE_WIDGET_FEATURE_COUNT + 2;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.ChartImpl <em>Chart</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.ChartImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getChart()
   * @generated
   */
  int CHART = 10;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int CHART__ITEM = NON_LINKABLE_WIDGET__ITEM;

  /**
   * The feature id for the '<em><b>Label</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int CHART__LABEL = NON_LINKABLE_WIDGET__LABEL;

  /**
   * The feature id for the '<em><b>Icon</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int CHART__ICON = NON_LINKABLE_WIDGET__ICON;

  /**
   * The feature id for the '<em><b>Label Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int CHART__LABEL_COLOR = NON_LINKABLE_WIDGET__LABEL_COLOR;

  /**
   * The feature id for the '<em><b>Value Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int CHART__VALUE_COLOR = NON_LINKABLE_WIDGET__VALUE_COLOR;

  /**
   * The feature id for the '<em><b>Visibility</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int CHART__VISIBILITY = NON_LINKABLE_WIDGET__VISIBILITY;

  /**
   * The feature id for the '<em><b>Service</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int CHART__SERVICE = NON_LINKABLE_WIDGET_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Refresh</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int CHART__REFRESH = NON_LINKABLE_WIDGET_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Period</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int CHART__PERIOD = NON_LINKABLE_WIDGET_FEATURE_COUNT + 2;

  /**
   * The number of structural features of the '<em>Chart</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int CHART_FEATURE_COUNT = NON_LINKABLE_WIDGET_FEATURE_COUNT + 3;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.WebviewImpl <em>Webview</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.WebviewImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getWebview()
   * @generated
   */
  int WEBVIEW = 11;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int WEBVIEW__ITEM = NON_LINKABLE_WIDGET__ITEM;

  /**
   * The feature id for the '<em><b>Label</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int WEBVIEW__LABEL = NON_LINKABLE_WIDGET__LABEL;

  /**
   * The feature id for the '<em><b>Icon</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int WEBVIEW__ICON = NON_LINKABLE_WIDGET__ICON;

  /**
   * The feature id for the '<em><b>Label Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int WEBVIEW__LABEL_COLOR = NON_LINKABLE_WIDGET__LABEL_COLOR;

  /**
   * The feature id for the '<em><b>Value Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int WEBVIEW__VALUE_COLOR = NON_LINKABLE_WIDGET__VALUE_COLOR;

  /**
   * The feature id for the '<em><b>Visibility</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int WEBVIEW__VISIBILITY = NON_LINKABLE_WIDGET__VISIBILITY;

  /**
   * The feature id for the '<em><b>Height</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int WEBVIEW__HEIGHT = NON_LINKABLE_WIDGET_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Url</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int WEBVIEW__URL = NON_LINKABLE_WIDGET_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Webview</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int WEBVIEW_FEATURE_COUNT = NON_LINKABLE_WIDGET_FEATURE_COUNT + 2;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.SwitchImpl <em>Switch</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.SwitchImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getSwitch()
   * @generated
   */
  int SWITCH = 12;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SWITCH__ITEM = NON_LINKABLE_WIDGET__ITEM;

  /**
   * The feature id for the '<em><b>Label</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SWITCH__LABEL = NON_LINKABLE_WIDGET__LABEL;

  /**
   * The feature id for the '<em><b>Icon</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SWITCH__ICON = NON_LINKABLE_WIDGET__ICON;

  /**
   * The feature id for the '<em><b>Label Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SWITCH__LABEL_COLOR = NON_LINKABLE_WIDGET__LABEL_COLOR;

  /**
   * The feature id for the '<em><b>Value Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SWITCH__VALUE_COLOR = NON_LINKABLE_WIDGET__VALUE_COLOR;

  /**
   * The feature id for the '<em><b>Visibility</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SWITCH__VISIBILITY = NON_LINKABLE_WIDGET__VISIBILITY;

  /**
   * The feature id for the '<em><b>Mappings</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SWITCH__MAPPINGS = NON_LINKABLE_WIDGET_FEATURE_COUNT + 0;

  /**
   * The number of structural features of the '<em>Switch</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SWITCH_FEATURE_COUNT = NON_LINKABLE_WIDGET_FEATURE_COUNT + 1;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.SliderImpl <em>Slider</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.SliderImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getSlider()
   * @generated
   */
  int SLIDER = 13;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SLIDER__ITEM = NON_LINKABLE_WIDGET__ITEM;

  /**
   * The feature id for the '<em><b>Label</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SLIDER__LABEL = NON_LINKABLE_WIDGET__LABEL;

  /**
   * The feature id for the '<em><b>Icon</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SLIDER__ICON = NON_LINKABLE_WIDGET__ICON;

  /**
   * The feature id for the '<em><b>Label Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SLIDER__LABEL_COLOR = NON_LINKABLE_WIDGET__LABEL_COLOR;

  /**
   * The feature id for the '<em><b>Value Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SLIDER__VALUE_COLOR = NON_LINKABLE_WIDGET__VALUE_COLOR;

  /**
   * The feature id for the '<em><b>Visibility</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SLIDER__VISIBILITY = NON_LINKABLE_WIDGET__VISIBILITY;

  /**
   * The feature id for the '<em><b>Frequency</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SLIDER__FREQUENCY = NON_LINKABLE_WIDGET_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Switch Enabled</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SLIDER__SWITCH_ENABLED = NON_LINKABLE_WIDGET_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Slider</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SLIDER_FEATURE_COUNT = NON_LINKABLE_WIDGET_FEATURE_COUNT + 2;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.SelectionImpl <em>Selection</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.SelectionImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getSelection()
   * @generated
   */
  int SELECTION = 14;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SELECTION__ITEM = NON_LINKABLE_WIDGET__ITEM;

  /**
   * The feature id for the '<em><b>Label</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SELECTION__LABEL = NON_LINKABLE_WIDGET__LABEL;

  /**
   * The feature id for the '<em><b>Icon</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SELECTION__ICON = NON_LINKABLE_WIDGET__ICON;

  /**
   * The feature id for the '<em><b>Label Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SELECTION__LABEL_COLOR = NON_LINKABLE_WIDGET__LABEL_COLOR;

  /**
   * The feature id for the '<em><b>Value Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SELECTION__VALUE_COLOR = NON_LINKABLE_WIDGET__VALUE_COLOR;

  /**
   * The feature id for the '<em><b>Visibility</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SELECTION__VISIBILITY = NON_LINKABLE_WIDGET__VISIBILITY;

  /**
   * The feature id for the '<em><b>Mappings</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SELECTION__MAPPINGS = NON_LINKABLE_WIDGET_FEATURE_COUNT + 0;

  /**
   * The number of structural features of the '<em>Selection</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SELECTION_FEATURE_COUNT = NON_LINKABLE_WIDGET_FEATURE_COUNT + 1;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.ListImpl <em>List</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.ListImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getList()
   * @generated
   */
  int LIST = 15;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int LIST__ITEM = NON_LINKABLE_WIDGET__ITEM;

  /**
   * The feature id for the '<em><b>Label</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int LIST__LABEL = NON_LINKABLE_WIDGET__LABEL;

  /**
   * The feature id for the '<em><b>Icon</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int LIST__ICON = NON_LINKABLE_WIDGET__ICON;

  /**
   * The feature id for the '<em><b>Label Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int LIST__LABEL_COLOR = NON_LINKABLE_WIDGET__LABEL_COLOR;

  /**
   * The feature id for the '<em><b>Value Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int LIST__VALUE_COLOR = NON_LINKABLE_WIDGET__VALUE_COLOR;

  /**
   * The feature id for the '<em><b>Visibility</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int LIST__VISIBILITY = NON_LINKABLE_WIDGET__VISIBILITY;

  /**
   * The feature id for the '<em><b>Separator</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int LIST__SEPARATOR = NON_LINKABLE_WIDGET_FEATURE_COUNT + 0;

  /**
   * The number of structural features of the '<em>List</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int LIST_FEATURE_COUNT = NON_LINKABLE_WIDGET_FEATURE_COUNT + 1;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.SetpointImpl <em>Setpoint</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.SetpointImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getSetpoint()
   * @generated
   */
  int SETPOINT = 16;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SETPOINT__ITEM = NON_LINKABLE_WIDGET__ITEM;

  /**
   * The feature id for the '<em><b>Label</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SETPOINT__LABEL = NON_LINKABLE_WIDGET__LABEL;

  /**
   * The feature id for the '<em><b>Icon</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SETPOINT__ICON = NON_LINKABLE_WIDGET__ICON;

  /**
   * The feature id for the '<em><b>Label Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SETPOINT__LABEL_COLOR = NON_LINKABLE_WIDGET__LABEL_COLOR;

  /**
   * The feature id for the '<em><b>Value Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SETPOINT__VALUE_COLOR = NON_LINKABLE_WIDGET__VALUE_COLOR;

  /**
   * The feature id for the '<em><b>Visibility</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SETPOINT__VISIBILITY = NON_LINKABLE_WIDGET__VISIBILITY;

  /**
   * The feature id for the '<em><b>Min Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SETPOINT__MIN_VALUE = NON_LINKABLE_WIDGET_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Max Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SETPOINT__MAX_VALUE = NON_LINKABLE_WIDGET_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Step</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SETPOINT__STEP = NON_LINKABLE_WIDGET_FEATURE_COUNT + 2;

  /**
   * The number of structural features of the '<em>Setpoint</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int SETPOINT_FEATURE_COUNT = NON_LINKABLE_WIDGET_FEATURE_COUNT + 3;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.ColorpickerImpl <em>Colorpicker</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.ColorpickerImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getColorpicker()
   * @generated
   */
  int COLORPICKER = 17;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int COLORPICKER__ITEM = NON_LINKABLE_WIDGET__ITEM;

  /**
   * The feature id for the '<em><b>Label</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int COLORPICKER__LABEL = NON_LINKABLE_WIDGET__LABEL;

  /**
   * The feature id for the '<em><b>Icon</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int COLORPICKER__ICON = NON_LINKABLE_WIDGET__ICON;

  /**
   * The feature id for the '<em><b>Label Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int COLORPICKER__LABEL_COLOR = NON_LINKABLE_WIDGET__LABEL_COLOR;

  /**
   * The feature id for the '<em><b>Value Color</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int COLORPICKER__VALUE_COLOR = NON_LINKABLE_WIDGET__VALUE_COLOR;

  /**
   * The feature id for the '<em><b>Visibility</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int COLORPICKER__VISIBILITY = NON_LINKABLE_WIDGET__VISIBILITY;

  /**
   * The feature id for the '<em><b>Frequency</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int COLORPICKER__FREQUENCY = NON_LINKABLE_WIDGET_FEATURE_COUNT + 0;

  /**
   * The number of structural features of the '<em>Colorpicker</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int COLORPICKER_FEATURE_COUNT = NON_LINKABLE_WIDGET_FEATURE_COUNT + 1;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.MappingImpl <em>Mapping</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.MappingImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getMapping()
   * @generated
   */
  int MAPPING = 18;

  /**
   * The feature id for the '<em><b>Cmd</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int MAPPING__CMD = 0;

  /**
   * The feature id for the '<em><b>Label</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int MAPPING__LABEL = 1;

  /**
   * The number of structural features of the '<em>Mapping</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int MAPPING_FEATURE_COUNT = 2;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.VisibilityRuleImpl <em>Visibility Rule</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.VisibilityRuleImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getVisibilityRule()
   * @generated
   */
  int VISIBILITY_RULE = 19;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int VISIBILITY_RULE__ITEM = 0;

  /**
   * The feature id for the '<em><b>Condition</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int VISIBILITY_RULE__CONDITION = 1;

  /**
   * The feature id for the '<em><b>Sign</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int VISIBILITY_RULE__SIGN = 2;

  /**
   * The feature id for the '<em><b>State</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int VISIBILITY_RULE__STATE = 3;

  /**
   * The number of structural features of the '<em>Visibility Rule</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int VISIBILITY_RULE_FEATURE_COUNT = 4;

  /**
   * The meta object id for the '{@link org.openhab.model.sitemap.impl.ColorArrayImpl <em>Color Array</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see org.openhab.model.sitemap.impl.ColorArrayImpl
   * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getColorArray()
   * @generated
   */
  int COLOR_ARRAY = 20;

  /**
   * The feature id for the '<em><b>Item</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int COLOR_ARRAY__ITEM = 0;

  /**
   * The feature id for the '<em><b>Condition</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int COLOR_ARRAY__CONDITION = 1;

  /**
   * The feature id for the '<em><b>Sign</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int COLOR_ARRAY__SIGN = 2;

  /**
   * The feature id for the '<em><b>State</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int COLOR_ARRAY__STATE = 3;

  /**
   * The feature id for the '<em><b>Arg</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int COLOR_ARRAY__ARG = 4;

  /**
   * The number of structural features of the '<em>Color Array</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
  int COLOR_ARRAY_FEATURE_COUNT = 5;


  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.SitemapModel <em>Model</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Model</em>'.
   * @see org.openhab.model.sitemap.SitemapModel
   * @generated
   */
  EClass getSitemapModel();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.Sitemap <em>Sitemap</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Sitemap</em>'.
   * @see org.openhab.model.sitemap.Sitemap
   * @generated
   */
  EClass getSitemap();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Sitemap#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see org.openhab.model.sitemap.Sitemap#getName()
   * @see #getSitemap()
   * @generated
   */
  EAttribute getSitemap_Name();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Sitemap#getLabel <em>Label</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Label</em>'.
   * @see org.openhab.model.sitemap.Sitemap#getLabel()
   * @see #getSitemap()
   * @generated
   */
  EAttribute getSitemap_Label();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Sitemap#getIcon <em>Icon</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Icon</em>'.
   * @see org.openhab.model.sitemap.Sitemap#getIcon()
   * @see #getSitemap()
   * @generated
   */
  EAttribute getSitemap_Icon();

  /**
   * Returns the meta object for the containment reference list '{@link org.openhab.model.sitemap.Sitemap#getChildren <em>Children</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the containment reference list '<em>Children</em>'.
   * @see org.openhab.model.sitemap.Sitemap#getChildren()
   * @see #getSitemap()
   * @generated
   */
  EReference getSitemap_Children();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.Widget <em>Widget</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Widget</em>'.
   * @see org.openhab.model.sitemap.Widget
   * @generated
   */
  EClass getWidget();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Widget#getItem <em>Item</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Item</em>'.
   * @see org.openhab.model.sitemap.Widget#getItem()
   * @see #getWidget()
   * @generated
   */
  EAttribute getWidget_Item();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Widget#getLabel <em>Label</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Label</em>'.
   * @see org.openhab.model.sitemap.Widget#getLabel()
   * @see #getWidget()
   * @generated
   */
  EAttribute getWidget_Label();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Widget#getIcon <em>Icon</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Icon</em>'.
   * @see org.openhab.model.sitemap.Widget#getIcon()
   * @see #getWidget()
   * @generated
   */
  EAttribute getWidget_Icon();

  /**
   * Returns the meta object for the containment reference list '{@link org.openhab.model.sitemap.Widget#getLabelColor <em>Label Color</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the containment reference list '<em>Label Color</em>'.
   * @see org.openhab.model.sitemap.Widget#getLabelColor()
   * @see #getWidget()
   * @generated
   */
  EReference getWidget_LabelColor();

  /**
   * Returns the meta object for the containment reference list '{@link org.openhab.model.sitemap.Widget#getValueColor <em>Value Color</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the containment reference list '<em>Value Color</em>'.
   * @see org.openhab.model.sitemap.Widget#getValueColor()
   * @see #getWidget()
   * @generated
   */
  EReference getWidget_ValueColor();

  /**
   * Returns the meta object for the containment reference list '{@link org.openhab.model.sitemap.Widget#getVisibility <em>Visibility</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the containment reference list '<em>Visibility</em>'.
   * @see org.openhab.model.sitemap.Widget#getVisibility()
   * @see #getWidget()
   * @generated
   */
  EReference getWidget_Visibility();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.NonLinkableWidget <em>Non Linkable Widget</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Non Linkable Widget</em>'.
   * @see org.openhab.model.sitemap.NonLinkableWidget
   * @generated
   */
  EClass getNonLinkableWidget();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.LinkableWidget <em>Linkable Widget</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Linkable Widget</em>'.
   * @see org.openhab.model.sitemap.LinkableWidget
   * @generated
   */
  EClass getLinkableWidget();

  /**
   * Returns the meta object for the containment reference list '{@link org.openhab.model.sitemap.LinkableWidget#getChildren <em>Children</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the containment reference list '<em>Children</em>'.
   * @see org.openhab.model.sitemap.LinkableWidget#getChildren()
   * @see #getLinkableWidget()
   * @generated
   */
  EReference getLinkableWidget_Children();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.Frame <em>Frame</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Frame</em>'.
   * @see org.openhab.model.sitemap.Frame
   * @generated
   */
  EClass getFrame();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.Text <em>Text</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Text</em>'.
   * @see org.openhab.model.sitemap.Text
   * @generated
   */
  EClass getText();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.Group <em>Group</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Group</em>'.
   * @see org.openhab.model.sitemap.Group
   * @generated
   */
  EClass getGroup();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.Image <em>Image</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Image</em>'.
   * @see org.openhab.model.sitemap.Image
   * @generated
   */
  EClass getImage();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Image#getUrl <em>Url</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Url</em>'.
   * @see org.openhab.model.sitemap.Image#getUrl()
   * @see #getImage()
   * @generated
   */
  EAttribute getImage_Url();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Image#getRefresh <em>Refresh</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Refresh</em>'.
   * @see org.openhab.model.sitemap.Image#getRefresh()
   * @see #getImage()
   * @generated
   */
  EAttribute getImage_Refresh();

  /**
   * Returns the meta object for the containment reference list '{@link org.openhab.model.sitemap.Image#getIconColor <em>Icon Color</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the containment reference list '<em>Icon Color</em>'.
   * @see org.openhab.model.sitemap.Image#getIconColor()
   * @see #getImage()
   * @generated
   */
  EReference getImage_IconColor();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.Video <em>Video</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Video</em>'.
   * @see org.openhab.model.sitemap.Video
   * @generated
   */
  EClass getVideo();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Video#getUrl <em>Url</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Url</em>'.
   * @see org.openhab.model.sitemap.Video#getUrl()
   * @see #getVideo()
   * @generated
   */
  EAttribute getVideo_Url();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Video#getEncoding <em>Encoding</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Encoding</em>'.
   * @see org.openhab.model.sitemap.Video#getEncoding()
   * @see #getVideo()
   * @generated
   */
  EAttribute getVideo_Encoding();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.Chart <em>Chart</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Chart</em>'.
   * @see org.openhab.model.sitemap.Chart
   * @generated
   */
  EClass getChart();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Chart#getService <em>Service</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Service</em>'.
   * @see org.openhab.model.sitemap.Chart#getService()
   * @see #getChart()
   * @generated
   */
  EAttribute getChart_Service();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Chart#getRefresh <em>Refresh</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Refresh</em>'.
   * @see org.openhab.model.sitemap.Chart#getRefresh()
   * @see #getChart()
   * @generated
   */
  EAttribute getChart_Refresh();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Chart#getPeriod <em>Period</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Period</em>'.
   * @see org.openhab.model.sitemap.Chart#getPeriod()
   * @see #getChart()
   * @generated
   */
  EAttribute getChart_Period();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.Webview <em>Webview</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Webview</em>'.
   * @see org.openhab.model.sitemap.Webview
   * @generated
   */
  EClass getWebview();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Webview#getHeight <em>Height</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Height</em>'.
   * @see org.openhab.model.sitemap.Webview#getHeight()
   * @see #getWebview()
   * @generated
   */
  EAttribute getWebview_Height();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Webview#getUrl <em>Url</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Url</em>'.
   * @see org.openhab.model.sitemap.Webview#getUrl()
   * @see #getWebview()
   * @generated
   */
  EAttribute getWebview_Url();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.Switch <em>Switch</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Switch</em>'.
   * @see org.openhab.model.sitemap.Switch
   * @generated
   */
  EClass getSwitch();

  /**
   * Returns the meta object for the containment reference list '{@link org.openhab.model.sitemap.Switch#getMappings <em>Mappings</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the containment reference list '<em>Mappings</em>'.
   * @see org.openhab.model.sitemap.Switch#getMappings()
   * @see #getSwitch()
   * @generated
   */
  EReference getSwitch_Mappings();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.Slider <em>Slider</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Slider</em>'.
   * @see org.openhab.model.sitemap.Slider
   * @generated
   */
  EClass getSlider();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Slider#getFrequency <em>Frequency</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Frequency</em>'.
   * @see org.openhab.model.sitemap.Slider#getFrequency()
   * @see #getSlider()
   * @generated
   */
  EAttribute getSlider_Frequency();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Slider#isSwitchEnabled <em>Switch Enabled</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Switch Enabled</em>'.
   * @see org.openhab.model.sitemap.Slider#isSwitchEnabled()
   * @see #getSlider()
   * @generated
   */
  EAttribute getSlider_SwitchEnabled();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.Selection <em>Selection</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Selection</em>'.
   * @see org.openhab.model.sitemap.Selection
   * @generated
   */
  EClass getSelection();

  /**
   * Returns the meta object for the containment reference list '{@link org.openhab.model.sitemap.Selection#getMappings <em>Mappings</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the containment reference list '<em>Mappings</em>'.
   * @see org.openhab.model.sitemap.Selection#getMappings()
   * @see #getSelection()
   * @generated
   */
  EReference getSelection_Mappings();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.List <em>List</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>List</em>'.
   * @see org.openhab.model.sitemap.List
   * @generated
   */
  EClass getList();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.List#getSeparator <em>Separator</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Separator</em>'.
   * @see org.openhab.model.sitemap.List#getSeparator()
   * @see #getList()
   * @generated
   */
  EAttribute getList_Separator();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.Setpoint <em>Setpoint</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Setpoint</em>'.
   * @see org.openhab.model.sitemap.Setpoint
   * @generated
   */
  EClass getSetpoint();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Setpoint#getMinValue <em>Min Value</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Min Value</em>'.
   * @see org.openhab.model.sitemap.Setpoint#getMinValue()
   * @see #getSetpoint()
   * @generated
   */
  EAttribute getSetpoint_MinValue();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Setpoint#getMaxValue <em>Max Value</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Max Value</em>'.
   * @see org.openhab.model.sitemap.Setpoint#getMaxValue()
   * @see #getSetpoint()
   * @generated
   */
  EAttribute getSetpoint_MaxValue();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Setpoint#getStep <em>Step</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Step</em>'.
   * @see org.openhab.model.sitemap.Setpoint#getStep()
   * @see #getSetpoint()
   * @generated
   */
  EAttribute getSetpoint_Step();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.Colorpicker <em>Colorpicker</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Colorpicker</em>'.
   * @see org.openhab.model.sitemap.Colorpicker
   * @generated
   */
  EClass getColorpicker();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Colorpicker#getFrequency <em>Frequency</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Frequency</em>'.
   * @see org.openhab.model.sitemap.Colorpicker#getFrequency()
   * @see #getColorpicker()
   * @generated
   */
  EAttribute getColorpicker_Frequency();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.Mapping <em>Mapping</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Mapping</em>'.
   * @see org.openhab.model.sitemap.Mapping
   * @generated
   */
  EClass getMapping();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Mapping#getCmd <em>Cmd</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Cmd</em>'.
   * @see org.openhab.model.sitemap.Mapping#getCmd()
   * @see #getMapping()
   * @generated
   */
  EAttribute getMapping_Cmd();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.Mapping#getLabel <em>Label</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Label</em>'.
   * @see org.openhab.model.sitemap.Mapping#getLabel()
   * @see #getMapping()
   * @generated
   */
  EAttribute getMapping_Label();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.VisibilityRule <em>Visibility Rule</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Visibility Rule</em>'.
   * @see org.openhab.model.sitemap.VisibilityRule
   * @generated
   */
  EClass getVisibilityRule();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.VisibilityRule#getItem <em>Item</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Item</em>'.
   * @see org.openhab.model.sitemap.VisibilityRule#getItem()
   * @see #getVisibilityRule()
   * @generated
   */
  EAttribute getVisibilityRule_Item();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.VisibilityRule#getCondition <em>Condition</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Condition</em>'.
   * @see org.openhab.model.sitemap.VisibilityRule#getCondition()
   * @see #getVisibilityRule()
   * @generated
   */
  EAttribute getVisibilityRule_Condition();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.VisibilityRule#getSign <em>Sign</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Sign</em>'.
   * @see org.openhab.model.sitemap.VisibilityRule#getSign()
   * @see #getVisibilityRule()
   * @generated
   */
  EAttribute getVisibilityRule_Sign();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.VisibilityRule#getState <em>State</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>State</em>'.
   * @see org.openhab.model.sitemap.VisibilityRule#getState()
   * @see #getVisibilityRule()
   * @generated
   */
  EAttribute getVisibilityRule_State();

  /**
   * Returns the meta object for class '{@link org.openhab.model.sitemap.ColorArray <em>Color Array</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for class '<em>Color Array</em>'.
   * @see org.openhab.model.sitemap.ColorArray
   * @generated
   */
  EClass getColorArray();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.ColorArray#getItem <em>Item</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Item</em>'.
   * @see org.openhab.model.sitemap.ColorArray#getItem()
   * @see #getColorArray()
   * @generated
   */
  EAttribute getColorArray_Item();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.ColorArray#getCondition <em>Condition</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Condition</em>'.
   * @see org.openhab.model.sitemap.ColorArray#getCondition()
   * @see #getColorArray()
   * @generated
   */
  EAttribute getColorArray_Condition();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.ColorArray#getSign <em>Sign</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Sign</em>'.
   * @see org.openhab.model.sitemap.ColorArray#getSign()
   * @see #getColorArray()
   * @generated
   */
  EAttribute getColorArray_Sign();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.ColorArray#getState <em>State</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>State</em>'.
   * @see org.openhab.model.sitemap.ColorArray#getState()
   * @see #getColorArray()
   * @generated
   */
  EAttribute getColorArray_State();

  /**
   * Returns the meta object for the attribute '{@link org.openhab.model.sitemap.ColorArray#getArg <em>Arg</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the meta object for the attribute '<em>Arg</em>'.
   * @see org.openhab.model.sitemap.ColorArray#getArg()
   * @see #getColorArray()
   * @generated
   */
  EAttribute getColorArray_Arg();

  /**
   * Returns the factory that creates the instances of the model.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @return the factory that creates the instances of the model.
   * @generated
   */
  SitemapFactory getSitemapFactory();

  /**
   * <!-- begin-user-doc -->
   * Defines literals for the meta objects that represent
   * <ul>
   *   <li>each class,</li>
   *   <li>each feature of each class,</li>
   *   <li>each enum,</li>
   *   <li>and each data type</li>
   * </ul>
   * <!-- end-user-doc -->
   * @generated
   */
  interface Literals
  {
    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.SitemapModelImpl <em>Model</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.SitemapModelImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getSitemapModel()
     * @generated
     */
    EClass SITEMAP_MODEL = eINSTANCE.getSitemapModel();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.SitemapImpl <em>Sitemap</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.SitemapImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getSitemap()
     * @generated
     */
    EClass SITEMAP = eINSTANCE.getSitemap();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute SITEMAP__NAME = eINSTANCE.getSitemap_Name();

    /**
     * The meta object literal for the '<em><b>Label</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute SITEMAP__LABEL = eINSTANCE.getSitemap_Label();

    /**
     * The meta object literal for the '<em><b>Icon</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute SITEMAP__ICON = eINSTANCE.getSitemap_Icon();

    /**
     * The meta object literal for the '<em><b>Children</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EReference SITEMAP__CHILDREN = eINSTANCE.getSitemap_Children();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.WidgetImpl <em>Widget</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.WidgetImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getWidget()
     * @generated
     */
    EClass WIDGET = eINSTANCE.getWidget();

    /**
     * The meta object literal for the '<em><b>Item</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute WIDGET__ITEM = eINSTANCE.getWidget_Item();

    /**
     * The meta object literal for the '<em><b>Label</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute WIDGET__LABEL = eINSTANCE.getWidget_Label();

    /**
     * The meta object literal for the '<em><b>Icon</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute WIDGET__ICON = eINSTANCE.getWidget_Icon();

    /**
     * The meta object literal for the '<em><b>Label Color</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EReference WIDGET__LABEL_COLOR = eINSTANCE.getWidget_LabelColor();

    /**
     * The meta object literal for the '<em><b>Value Color</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EReference WIDGET__VALUE_COLOR = eINSTANCE.getWidget_ValueColor();

    /**
     * The meta object literal for the '<em><b>Visibility</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EReference WIDGET__VISIBILITY = eINSTANCE.getWidget_Visibility();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.NonLinkableWidgetImpl <em>Non Linkable Widget</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.NonLinkableWidgetImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getNonLinkableWidget()
     * @generated
     */
    EClass NON_LINKABLE_WIDGET = eINSTANCE.getNonLinkableWidget();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.LinkableWidgetImpl <em>Linkable Widget</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.LinkableWidgetImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getLinkableWidget()
     * @generated
     */
    EClass LINKABLE_WIDGET = eINSTANCE.getLinkableWidget();

    /**
     * The meta object literal for the '<em><b>Children</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EReference LINKABLE_WIDGET__CHILDREN = eINSTANCE.getLinkableWidget_Children();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.FrameImpl <em>Frame</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.FrameImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getFrame()
     * @generated
     */
    EClass FRAME = eINSTANCE.getFrame();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.TextImpl <em>Text</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.TextImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getText()
     * @generated
     */
    EClass TEXT = eINSTANCE.getText();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.GroupImpl <em>Group</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.GroupImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getGroup()
     * @generated
     */
    EClass GROUP = eINSTANCE.getGroup();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.ImageImpl <em>Image</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.ImageImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getImage()
     * @generated
     */
    EClass IMAGE = eINSTANCE.getImage();

    /**
     * The meta object literal for the '<em><b>Url</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute IMAGE__URL = eINSTANCE.getImage_Url();

    /**
     * The meta object literal for the '<em><b>Refresh</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute IMAGE__REFRESH = eINSTANCE.getImage_Refresh();

    /**
     * The meta object literal for the '<em><b>Icon Color</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EReference IMAGE__ICON_COLOR = eINSTANCE.getImage_IconColor();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.VideoImpl <em>Video</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.VideoImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getVideo()
     * @generated
     */
    EClass VIDEO = eINSTANCE.getVideo();

    /**
     * The meta object literal for the '<em><b>Url</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute VIDEO__URL = eINSTANCE.getVideo_Url();

    /**
     * The meta object literal for the '<em><b>Encoding</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute VIDEO__ENCODING = eINSTANCE.getVideo_Encoding();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.ChartImpl <em>Chart</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.ChartImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getChart()
     * @generated
     */
    EClass CHART = eINSTANCE.getChart();

    /**
     * The meta object literal for the '<em><b>Service</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute CHART__SERVICE = eINSTANCE.getChart_Service();

    /**
     * The meta object literal for the '<em><b>Refresh</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute CHART__REFRESH = eINSTANCE.getChart_Refresh();

    /**
     * The meta object literal for the '<em><b>Period</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute CHART__PERIOD = eINSTANCE.getChart_Period();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.WebviewImpl <em>Webview</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.WebviewImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getWebview()
     * @generated
     */
    EClass WEBVIEW = eINSTANCE.getWebview();

    /**
     * The meta object literal for the '<em><b>Height</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute WEBVIEW__HEIGHT = eINSTANCE.getWebview_Height();

    /**
     * The meta object literal for the '<em><b>Url</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute WEBVIEW__URL = eINSTANCE.getWebview_Url();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.SwitchImpl <em>Switch</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.SwitchImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getSwitch()
     * @generated
     */
    EClass SWITCH = eINSTANCE.getSwitch();

    /**
     * The meta object literal for the '<em><b>Mappings</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EReference SWITCH__MAPPINGS = eINSTANCE.getSwitch_Mappings();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.SliderImpl <em>Slider</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.SliderImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getSlider()
     * @generated
     */
    EClass SLIDER = eINSTANCE.getSlider();

    /**
     * The meta object literal for the '<em><b>Frequency</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute SLIDER__FREQUENCY = eINSTANCE.getSlider_Frequency();

    /**
     * The meta object literal for the '<em><b>Switch Enabled</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute SLIDER__SWITCH_ENABLED = eINSTANCE.getSlider_SwitchEnabled();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.SelectionImpl <em>Selection</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.SelectionImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getSelection()
     * @generated
     */
    EClass SELECTION = eINSTANCE.getSelection();

    /**
     * The meta object literal for the '<em><b>Mappings</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EReference SELECTION__MAPPINGS = eINSTANCE.getSelection_Mappings();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.ListImpl <em>List</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.ListImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getList()
     * @generated
     */
    EClass LIST = eINSTANCE.getList();

    /**
     * The meta object literal for the '<em><b>Separator</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute LIST__SEPARATOR = eINSTANCE.getList_Separator();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.SetpointImpl <em>Setpoint</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.SetpointImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getSetpoint()
     * @generated
     */
    EClass SETPOINT = eINSTANCE.getSetpoint();

    /**
     * The meta object literal for the '<em><b>Min Value</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute SETPOINT__MIN_VALUE = eINSTANCE.getSetpoint_MinValue();

    /**
     * The meta object literal for the '<em><b>Max Value</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute SETPOINT__MAX_VALUE = eINSTANCE.getSetpoint_MaxValue();

    /**
     * The meta object literal for the '<em><b>Step</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute SETPOINT__STEP = eINSTANCE.getSetpoint_Step();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.ColorpickerImpl <em>Colorpicker</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.ColorpickerImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getColorpicker()
     * @generated
     */
    EClass COLORPICKER = eINSTANCE.getColorpicker();

    /**
     * The meta object literal for the '<em><b>Frequency</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute COLORPICKER__FREQUENCY = eINSTANCE.getColorpicker_Frequency();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.MappingImpl <em>Mapping</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.MappingImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getMapping()
     * @generated
     */
    EClass MAPPING = eINSTANCE.getMapping();

    /**
     * The meta object literal for the '<em><b>Cmd</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute MAPPING__CMD = eINSTANCE.getMapping_Cmd();

    /**
     * The meta object literal for the '<em><b>Label</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute MAPPING__LABEL = eINSTANCE.getMapping_Label();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.VisibilityRuleImpl <em>Visibility Rule</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.VisibilityRuleImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getVisibilityRule()
     * @generated
     */
    EClass VISIBILITY_RULE = eINSTANCE.getVisibilityRule();

    /**
     * The meta object literal for the '<em><b>Item</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute VISIBILITY_RULE__ITEM = eINSTANCE.getVisibilityRule_Item();

    /**
     * The meta object literal for the '<em><b>Condition</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute VISIBILITY_RULE__CONDITION = eINSTANCE.getVisibilityRule_Condition();

    /**
     * The meta object literal for the '<em><b>Sign</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute VISIBILITY_RULE__SIGN = eINSTANCE.getVisibilityRule_Sign();

    /**
     * The meta object literal for the '<em><b>State</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute VISIBILITY_RULE__STATE = eINSTANCE.getVisibilityRule_State();

    /**
     * The meta object literal for the '{@link org.openhab.model.sitemap.impl.ColorArrayImpl <em>Color Array</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.openhab.model.sitemap.impl.ColorArrayImpl
     * @see org.openhab.model.sitemap.impl.SitemapPackageImpl#getColorArray()
     * @generated
     */
    EClass COLOR_ARRAY = eINSTANCE.getColorArray();

    /**
     * The meta object literal for the '<em><b>Item</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute COLOR_ARRAY__ITEM = eINSTANCE.getColorArray_Item();

    /**
     * The meta object literal for the '<em><b>Condition</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute COLOR_ARRAY__CONDITION = eINSTANCE.getColorArray_Condition();

    /**
     * The meta object literal for the '<em><b>Sign</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute COLOR_ARRAY__SIGN = eINSTANCE.getColorArray_Sign();

    /**
     * The meta object literal for the '<em><b>State</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute COLOR_ARRAY__STATE = eINSTANCE.getColorArray_State();

    /**
     * The meta object literal for the '<em><b>Arg</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    EAttribute COLOR_ARRAY__ARG = eINSTANCE.getColorArray_Arg();

  }

} //SitemapPackage
