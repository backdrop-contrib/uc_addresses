<?php
/**
 * @file
 * The schema address class.
 *
 * This class creates an interface between the address data used by
 * Ubercart and the rest of uc_addresses. If the address model changes
 * in the future, our hope is that only this class will need to
 * change.
 */

class UcAddressesSchemaAddress {
  // -----------------------------------------------------------------------------
  // PROPERTIES
  // -----------------------------------------------------------------------------

  /**
   * The base schema address object.
   *
   * We extend this by aggregation.
   *
   * @var stdClass
   * @access private
   */
  private $schemaAddress;

  /**
   * TRUE if the address is changed after being loaded or created.
   *
   * @var boolean
   * @access private
   */
  private $dirty = FALSE;

  /**
   * A variable that's used to keep data when the address object
   * is being serialized.
   *
   * @var array
   * @access protected
   */
  protected $sleep = array();

  // -----------------------------------------------------------------------------
  // CONSTRUCT
  // -----------------------------------------------------------------------------

  /**
   * Construct a schema address.
   *
   * @param object $schemaAddress
   *	The schema address array to wrap. If null, a new stdClass
   * 	object is created.
   * @access public
   * @return void
   */
  public function __construct($schemaAddress = NULL) {
    $this->schemaAddress = new stdClass();
    if (is_object($schemaAddress)) {
      $this->schemaAddress = $schemaAddress;
    }
    // Make sure all fields are present
    $fields = uc_addresses_get_address_fields();
    foreach ($fields as $fieldname => $fielddata) {
      if (!isset($this->schemaAddress->$fieldname)) {
        $this->schemaAddress->$fieldname = '';
      }
    }
  }

  /**
   * Tells which members may be kept when the address is being serialized
   *
   * @access public
   * @return array
   */
  public function __sleep() {
    $vars = get_object_vars($this);
    foreach ($vars as $key => $value) {
      if ($key != 'sleep') {
        $this->sleep[$key] = $value;
      }
    }
    return array(
      'sleep',
    );
  }

  /**
   * Restore variables when the address is unserialized
   *
   * @access public
   * @return array
   */
  public function __wakeup() {
    // Restore variables saved in sleep
    foreach ($this->sleep as $key => $value) {
      $this->$key = $value;
    }
    // Clear out sleep
    $this->sleep = array();
  }

  // -----------------------------------------------------------------------------
  // "DIRTY" METHODS
  // -----------------------------------------------------------------------------

  /**
   * Clear the dirty flag.
   *
   * When set, the dirty flag signals that the address needs to be
   * saved in the database.
   *
   * @access protected
   * @return void
   */
  protected function clearDirty() {
    $this->dirty = FALSE;
  }

  /**
   * Set the dirty flag.
   *
   * When set, the dirty flag signals that the address needs to be
   * saved in the database.
   *
   * @access protected
   * @return void
   */
  protected function setDirty() {
    $this->dirty = TRUE;
  }

  /**
   * Reports if the address needs to be saved to the database.
   *
   * @access protected
   * @return boolean
   *	TRUE if the address needs to be saved to the database.
   */
  protected function isDirty() {
    return $this->dirty;
  }

  // -----------------------------------------------------------------------------
  // FIELDS
  // -----------------------------------------------------------------------------

  /**
   * Get a field's value.
   *
   * @param string $fieldName
   *	The name of the field whose value we want.
   * @access public
   * @return mixed
   *	The field value.
   * @throws UcAddressInvalidFieldException
   */
  public function getField($fieldName) {
    self::fieldMustExist($fieldName);
    return $this->schemaAddress->$fieldName;
  }

  /**
   * Set a field's value.
   *
   * @param string $fieldName
   *	 The name of the field whose value we will set.
   * @param mixed $value
   *	 The value to which to set the field.
   * @access public
   * @return void
   * @throws UcAddressInvalidFieldException
   */
  public function setField($fieldName, $value) {
    self::fieldMustExist($fieldName);
    if ($this->schemaAddress->$fieldName !== $value) {
      $this->schemaAddress->$fieldName = $value;
      $this->setDirty();
    }
  }

  /**
   * Returns TRUE if field is registered through the API
   *
   * @param string $fieldName
   *	 The name of the field whose existence we want to check.
   * @access public
   * @static
   * @return boolean
   *	 TRUE if addresses have a field with the given name.
   */
  static public function fieldExists($fieldName) {
    $fields_data = uc_addresses_get_address_fields();
    return isset($fields_data[$fieldName]);
  }

  /**
   * Throws an exception if the field does not exist.
   *
   * @param $fieldName The name of the field.
   * @access private
   * @static
   * @return void
   * @throws UcAddressInvalidFieldException
   */
  static private function fieldMustExist($fieldName) {
    if (!self::fieldExists($fieldName)) {
      throw new UcAddressesInvalidFieldException(t('Invalid field name %name', array('%name' => $fieldName)));
    }
  }

  /**
   * Returns "safe" field data
   *
   * @access public
   * @return array
   */
  public function getFieldData() {
    ctools_include('plugins');
    $values = array();
    $fields_data = uc_addresses_get_address_fields();
    foreach ($fields_data as $fieldname => $fielddata) {
      $class = ctools_plugin_load_class('uc_addresses', 'field_handlers', $fielddata['handler'], 'handler');
      $instance = new $class($fieldname, $this);
      $values[$fieldname] = $instance->outputValue($this->getField($fieldname));
    }
    return $values;
  }

  /**
   * Returns "raw" field data (contents of the schema address object)
   *
   * @access public
   * @return array
   */
  public function getRawFieldData() {
    return (array) $this->schemaAddress;
  }

  // -----------------------------------------------------------------------------
  // SCHEMA METHODS
  // -----------------------------------------------------------------------------

  /**
   * Get the aggregated schema address.
   *
   * @access protected
   * @return UcAddress
   *	The aggregated UcAddress.
   */
  protected function getSchemaAddress() {
    return $this->schemaAddress;
  }

  /**
   * Set the aggregated schema address.
   *
   * @param object $address
   *	The address object to wrap.
   * @access protected
   * @return void
   */
  protected function setSchemaAddress($address) {
    if (is_object($address)) {
      $this->schemaAddress = $address;
      $this->setDirty();
    }
  }

  /**
   * Returns TRUE if field is part of the schema
   *
   * @param string $fieldName
   *	The name of the field whose existence we want to check.
   * @access public
   * @static
   * @return boolean
   *	TRUE if addresses have a field with the given name.
   */
  static public function schemaFieldExists($fieldName) {
    $schema = drupal_get_schema('uc_addresses');
    if (!empty($schema['fields']) && is_array($schema['fields'])) {
      return isset($schema['fields'][$fieldName]);
    }
    return FALSE;
  }

  /**
   * Throws an exception if the schema field does not exist.
   *
   * @param $fieldName The name of the field.
   * @access private
   * @static
   * @return void
   * @throws UcAddressInvalidFieldException
   */
  static private function schemaFieldMustExist($fieldName) {
    if (!self::schemaFieldExists($fieldName)) {
      throw new UcAddressesInvalidFieldException(t('Invalid schema field name %name', array('%name' => $fieldName)));
    }
  }
}
