<?php
/**
 * @file
 * Contains the UcAddressesSchemaAddress class.
 */

/**
 * The schema address class.
 *
 * This is the base class for addresses. It's goal is to provide functionality
 * for address fields. You can get and set address field values. For this it's
 * connected with the field handler system. Unlike the UcAddressesAddress class
 * it's not connected with the address book class, which means you *could* use
 * this class if you want to make use of the field handler system and you want to
 * bypass any restrictions implied with UcAddressesAddress (such as having unique
 * nicknames), but in most cases you should not interact with this class directly.
 *
 * The class doesn't interact with the database itself: this should be done in
 * subclasses (such as UcAddressesAddress).
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
   * A variable that's used to keep data when the address object.
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
        $class = ctools_plugin_load_class('uc_addresses', 'field_handlers', $fielddata['handler'], 'handler');
        $instance = new $class($fieldname, $this);
        $this->schemaAddress->$fieldname = $instance->getDefaultValue();
      }
    }
  }

  /**
   * Tells which members may be kept when the address is being serialized.
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
   * Restore variables when the address is unserialized.
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
   *	 TRUE if the address needs to be saved to the database.
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
   *	 The name of the field whose value we want.
   * @access public
   * @return mixed
   *	 The field value.
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
   * Set multiple fields at once.
   *
   * @param array $fields
   *   An array of fields with $fieldName => $value.
   * @param boolean $fieldsMustExist
   *   (optional) If TRUE, every field in the array must exists.
   *   If there are fields in the array that do not exists an
   *   UcAddressInvalidFieldException will be thrown.
   *   Defaults to FALSE (no exceptions will be thrown).
   *
   * @access public
   * @return void
   * @throws UcAddressInvalidFieldException
   */
  public function setMultipleFields($fields, $fieldsMustExist = FALSE) {
    foreach ($fields as $fieldName => $value) {
      if (!$fieldsMustExist && !self::fieldExists($fieldName)) {
        continue;
      }
      $this->setField($fieldName, $value);
    }
  }

  /**
   * Returns TRUE if field is registered through the API.
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
   * Returns "safe" field data.
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
   * Returns "raw" field data (contents of the schema address object).
   *
   * @access public
   * @return array
   */
  public function getRawFieldData() {
    return (array) $this->schemaAddress;
  }

  /**
   * Get a "safe" field value from a single field.
   *
   * @param string $fieldName
   *	 The name of the field whose value we want.
   * @param string $format
   *   (optional) The format in which the value should be outputted.
   *   See outputValue() in UcAddressesFieldHandler.class.php for
   *   more information.
   * @param string $context
   *   (optional) The context where the field is used for.
   *
   * @access public
   * @return mixed
   *   The field's value safe for ouput.
   * @throws UcAddressInvalidFieldException
   */
  public function getFieldValue($fieldName, $format = '', $context = 'default') {
    self::fieldMustExist($fieldName);
    $handler = uc_addresses_get_address_field_handler($this, $fieldName, $context);
    return $handler->outputValue($this->getField($fieldName), $format);
  }

  // -----------------------------------------------------------------------------
  // SCHEMA METHODS
  // -----------------------------------------------------------------------------

  /**
   * Get the aggregated schema address.
   *
   * @access protected
   * @return object
   *	 The aggregated address object.
   */
  protected function getSchemaAddress() {
    return $this->schemaAddress;
  }

  /**
   * Set the aggregated schema address.
   *
   * @param object $address
   *	 The address object to wrap.
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
   * Returns TRUE if field is part of the schema.
   *
   * @param string $fieldName
   *	 The name of the field whose existence we want to check.
   * @access public
   * @static
   * @return boolean
   *	 TRUE if addresses have a field with the given name.
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

  /**
   * Checks if the schema address of the given address
   * is equal to the schema address of this.
   *
   * @param UcAddressesSchemaAddress $address
   * @access public
   * @return boolean
   */
  public function compareAddress(UcAddressesSchemaAddress $address) {
    static $fields_to_compare = array();

    if ($address === $this) {
      // No comparison needed. Given address object is exactly the same.
      return TRUE;
    }

    $fieldsDataThisAddress = $this->getRawFieldData();
    $fieldsDataOtherAddress = $address->getRawFieldData();

    // Find out which field to compare
    if (count($fields_to_compare) < 1) {
      $fields_data = uc_addresses_get_address_fields();
      foreach ($fields_data as $fieldname => $field_data) {
        if ($field_data['compare']) {
          $fields_to_compare[] = $fieldname;
        }
      }
    }

    foreach ($fields_to_compare as $fieldname) {
      if ($fieldsDataThisAddress[$fieldname] != $fieldsDataOtherAddress[$fieldname]) {
        return FALSE;
      }
    }
    return TRUE;
  }
}
