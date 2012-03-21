<?php
/**
 * @file
 * Contains the UcAddressesFieldHandler class.
 */

/**
 * Base class for fields used in Ubercart Addresses
 *
 * The field handler API is designed to add extra address fields, such as a gender
 * field. To add extra address fields, you'll need to declare a "field handler" in
 * your module and implement this handler. You will also need to declare one or
 * more "fields" that will be using that handler.
 */
abstract class UcAddressesFieldHandler {
  // -----------------------------------------------------------------------------
  // PROPERTIES
  // -----------------------------------------------------------------------------

  /**
   * Name of this field
   *
   * @var string
   * @access private
   */
  private $name;

  /**
   * Address object
   *
   * @var UcAddressesAddress
   * @access private
   */
  private $address;

  /**
   * The context in which this field is used.
   *
   * @var string
   * @access private
   */
  private $context;

  /**
   * The declared field definition.
   *
   * @var array
   * @access private
   */
  private $definition;

  // -----------------------------------------------------------------------------
  // CONSTRUCT
  // -----------------------------------------------------------------------------

  /**
   * UcAddressesFormField object constructor
   *
   * @param string $name
   *   The name of the address field
   * @param UcAddressesSchemaAddress $address
   *   Instance of UcAddressesSchemaAddress
   * @param string $context
   *   The context in which this field is used
   * @final
   * @access public
   * @return void
   */
  final public function __construct($name, UcAddressesSchemaAddress $address, $context = 'default') {
    $this->name = $name;
    $this->address = $address;
    if ($context) {
      $this->context = (string) $context;
    }
    else {
      $this->context = 'default';
    }

    // Load the definition for this field
    $fields = uc_addresses_get_address_fields();
    if (isset($fields[$this->name])) {
      $this->definition = $fields[$this->name];
    }
    else {
      $this->definition = array();
    }

    // Perform eventually further initialization.
    $this->init();
  }

  /**
   * Can be used by subclasses to do some initialization upon
   * construction of the object
   *
   * @access protected
   * @return void
   */
  protected function init() { }

  // -----------------------------------------------------------------------------
  // GETTERS
  // -----------------------------------------------------------------------------

  /**
   * Returns the field name
   *
   * @access public
   * @final
   * @return UcAddressesAddress
   */
  final public function getFieldName() {
    return $this->name;
  }

  /**
   * Returns the address attached to this field.
   * Generally used by subclasses to get the necessary address data
   *
   * @access public
   * @final
   * @return UcAddressesAddress
   */
  final public function getAddress() {
    return $this->address;
  }

  /**
   * Returns the context in which this field is used.
   *
   * @access public
   * @final
   * @return string
   */
  final public function getContext() {
    return $this->context;
  }

  /**
   * Returns a property from the field definition.
   *
   * If the property doesn't exists an exception will be thrown.
   *
   * @access public
   * @return mixed
   * @throws UcAddressesInvalidParameterException
   */
  final public function getProperty($name) {
    if (!isset($this->definition[$name])) {
      throw new UcAddressesInvalidParameterException(t('Property %property not found in the field definition for field %field.', array('%property' => $name, '%field' => $this->name)));
    }
    return $this->definition[$name];
  }

  /**
   * Returns the title to use when displaying a field.
   *
   * @access public
   * @abstract
   * @return string
   *	 The field title.
   */
  public function getFieldTitle() {
    return $this->getProperty('title');
  }

  /**
   * Returns a default value for this field.
   *
   * Subclasses can override this method to provide a default
   * value for their field.
   *
   * @access public
   * @return string
   */
  public function getDefaultValue() {
    return '';
  }

  // -----------------------------------------------------------------------------
  // ABSTRACT METHODS
  // -----------------------------------------------------------------------------

  /**
   * Returns the editable field
   *
   * @param array $form
   * @param array $form_values
   * @abstract
   * @access public
   * @return array
   */
  abstract public function getFormField($form, $form_values);

  /**
   * Check to see if a field is enabled.
   *
   * @access public
   * @abstract
   * @return boolean
   *	 TRUE if the field is enabled.
   */
  abstract public function isFieldEnabled();

  /**
   * Check to see if a field is required.
   *
   * @access public
   * @abstract
   * @return boolean
   *	 TRUE if the field is required.
   */
  abstract public function isFieldRequired();

  // -----------------------------------------------------------------------------
  // SETTERS
  // -----------------------------------------------------------------------------

  /**
   * Sets value in the address object.
   *
   * The default is that just setField() will be called,
   * but other field handlers may want to handle the
   * value differently.
   *
   * @param mixed $value
   * @access public
   * @return void
   */
  public function setValue($value) {
    $this->address->setField($this->name, $value);
  }

  // -----------------------------------------------------------------------------
  // ACTION
  // -----------------------------------------------------------------------------

  /**
   * Check a fields' value
   *
   * Can be used by subclasses to do some validation based on the value.
   *
   * @param mixed $value
   * @access public
   * @return void
   */
  public function validateValue(&$value) { }

  /**
   * Checks if field passes the context
   *
   * @access public
   * @return boolean
   */
  public function checkContext() {
    $fields = uc_addresses_get_address_fields();
    if (isset($fields[$this->name])) {
      $display_settings = $fields[$this->name]['display_settings'];
      if ((!isset($display_settings[$this->context]) && $display_settings['default']) || (isset($display_settings[$this->context]) && $display_settings[$this->context] == TRUE)) {
        return TRUE;
      }
      return FALSE;
    }
    return TRUE;
  }

  // -----------------------------------------------------------------------------
  // OUTPUT
  // -----------------------------------------------------------------------------

  /**
   * Returns supported tokens
   *
   * @return array
   */
  public function getTokenInfo() {
    $fieldname = $this->getFieldName();
    $tokens = array();

    // Check if handler supports multiple output formats
    $formats = $this->getOutputFormats();
    if (count($formats) > 0) {
      foreach ($formats as $format => $label) {
        $tokens[$fieldname . ':' . $format] = array(
          'name' => $format,
          'description' => $label,
        );
      }
    }
    $tokens[$fieldname] = array(
      'name' => $this->getFieldTitle(),
      'description' => $this->getProperty('description'),
    );
    return $tokens;
  }

  /**
   * Returns an array of possible output formats the handler supports.
   *
   * Should be overriden by handlers who have more than one way of outputting
   * a value.
   *
   * @access public
   * @return array
   */
  public function getOutputFormats() {
    return array();
  }

  /**
   * Output a fields value
   *
   * @param mixed $value
   *   The value to output
   * @param string $format
   *   The format in which the value should be outputted.
   *   Possible formats are declared by field handlers: getOutputFormats().
   * @access public
   * @return string
   * @see getOutputFormats()
   */
  public function outputValue($value = '', $format = '') {
    if ($value === '') {
      $value = $this->address->getField($this->name);
    }
    return check_plain($value);
  }
}
