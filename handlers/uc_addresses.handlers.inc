<?php
/**
 * @file
 * Field handlers for Ubercart Addresses address fields:
 * address_name, default_shipping, default_billing
 */

/**
 * Class for the address name field
 */
class UcAddressesAddressNameFieldHandler extends UcAddressesFieldHandler {
  /**
   * Returns the editable field
   *
   * @param array $form
   * @param array $form_values
   * @access public
   * @return array
   */
  public function getFormField($form, $form_values) {
    $fieldName = $this->getFieldName();
    $fieldValue = $this->getAddress()->getField($fieldName);
    $default = (isset($form_values[$fieldName]))? $form_values[$fieldName]:$fieldValue;

    return array(
      $fieldName => array(
        '#type' => 'textfield',
        '#title' => t('Save as'),
        '#description' => t('Enter a name for this address (e.g. Home or Work)'),
        '#default_value' => $default,
        '#required' => ($form['#required'] === FALSE)? FALSE:$this->isFieldRequired(),
        '#maxlength' => 20,
      ),
    );
  }

  /**
   * Returns the title to use when displaying the field.
   *
   * @access public
   * @return string
   *	 The field title.
   */
  public function getFieldTitle() {
    return t('Name');
  }

  /**
   * Check to see if the field is enabled.
   *
   * @access public
   * @return boolean
   *	 TRUE if the field is enabled.
   */
  public function isFieldEnabled() {
    return variable_get('uc_addresses_use_address_name', TRUE);
  }

  /**
   * Check to see if the field is required.
   *
   * @access public
   * @return boolean
   *	 TRUE if the field is required.
   */
  public function isFieldRequired() {
    return FALSE;
  }

  /**
   * Check if given name is unique
   *
   * @param mixed $value
   * @access public
   * @return void
   */
  public function validateValue($value) {
    $fieldName = $this->getFieldName();
    if ($value) {
      if (!$this->getAddress()->setName($value)) {
        form_set_error($fieldName, t('You already have an address named %name in your address book.', array('%name' => $value)));
      }
    }
  }
}

/**
 * Class for default address field
 */
class UcAddressesDefaultAddressFieldHandler extends UcAddressesFieldHandler {
  /**
   * Returns the editable field
   *
   * @param array $form
   * @param array $form_values
   * @abstract
   * @access public
   * @return array
   */
  public function getFormField($form, $form_values) {
    $fieldName = $this->getFieldName();
    $fieldValue = $this->getAddress()->getField($fieldName);
    $default = (isset($form_values[$fieldName]))? $form_values[$fieldName]:$fieldValue;
    $type = $this->getString('type');

    // Check if user already has a default address of type $type
    $default_address = $this->getAddress()->getAddressBook()->getDefaultAddress($type);

    if (!is_null($default_address)) {
      return array(
        $fieldName => array(
          '#type' => 'checkbox',
          '#title' => $this->getFieldTitle(),
          '#suffix' => $this->getString('suffix'),
          '#default_value' => $default,
          '#disabled' => $fieldValue,
          '#required' => ($form['#required'] === FALSE)? FALSE:$this->isFieldRequired(),
        ),
      );
    }
    else {
      // No default address of type $type yet. Force default.
      return array(
        $fieldName => array(
          '#type' => 'value',
          '#value' => 1,
        ),
      );
    }
  }

  /**
   * Returns the title to use when displaying the field.
   *
   * @access public
   * @return string
   *	 The field title.
   */
  public function getFieldTitle() {
    return $this->getString('title');
  }

  /**
   * Check to see if the field is enabled.
   *
   * @access public
   * @return boolean
   *	 TRUE if the field is enabled.
   */
  public function isFieldEnabled() {
    return variable_get('uc_addresses_use_default_' . $this->getString('type'), TRUE);
  }

  /**
   * Check to see if the field is required.
   *
   * @access public
   * @return boolean
   *	 TRUE if the field is required.
   */
  public function isFieldRequired() {
    return FALSE;
  }

  /**
   * Returns a default value for this field.
   *
   * @access public
   * @return int
   */
  public function getDefaultValue() {
    return 0;
  }
}

/**
 * Class for the created/modified field
 */
class UcAddressesDateFieldHandler extends UcAddressesFieldHandler {
  /**
   * Returns the editable field
   *
   * @param array $form
   * @param array $form_values
   * @abstract
   * @access public
   * @return array
   */
  public function getFormField($form, $form_values) {
    $fieldName = $this->getFieldName();
    $fieldValue = $this->getAddress()->getField($fieldName);
    $default = (isset($form_values[$fieldName]))? $form_values[$fieldName]:$fieldValue;
    if (is_numeric($default)) {
      $default = array(
        'hour' => date('H', $default),
        'minute' => date('i', $default),
        'second' => date('s', $default),
        'month' => date('n', $default),
        'day' => date('j', $default),
        'year' => date('Y', $default),
      );
    }

    return array(
      $fieldName => array(
        '#type' => 'date',
        '#title' => $this->getFieldTitle(),
        '#default_value' => $default,
        '#required' => ($form['#required'] === FALSE)? FALSE:$this->isFieldRequired(),
      ),
    );
  }

  /**
   * Check to see if the field is enabled.
   *
   * @access public
   * @return boolean
   *	 TRUE if the field is enabled.
   */
  public function getFieldTitle() {
    return $this->getString('title');
  }

  /**
   * Check to see if the field is enabled.
   *
   * @access public
   * @return boolean
   *	 TRUE if the field is enabled.
   */
  public function isFieldEnabled() {
    return TRUE;
  }

  /**
   * Check to see if the field is required.
   *
   * @access public
   * @return boolean
   *	 TRUE if the field is required.
   */
  public function isFieldRequired() {
    return FALSE;
  }

  // -----------------------------------------------------------------------------
  // VALIDATE
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
  public function validateValue(&$value) {
    $value = mktime(0, 0, 0, $value['month'], $value['day'], $value['year']);
  }

  // -----------------------------------------------------------------------------
  // OUTPUT
  // -----------------------------------------------------------------------------

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
      $value = $this->getAddress()->getField($this->getFieldName());
    }
    return format_date($value);
  }
}

/**
 * Class for aid/uid field (which can not be set directly)
 */
class UcAddressesHiddenFieldHandler extends UcAddressesFieldHandler {
  /**
   * Returns the editable field
   *
   * @param array $form
   * @param array $form_values
   * @abstract
   * @access public
   * @return array
   */
  public function getFormField($form, $form_values) {
    $fieldName = $this->getFieldName();
    $fieldValue = $this->getAddress()->getField($fieldName);
    $default = (isset($form_values[$fieldName]))? $form_values[$fieldName]:$fieldValue;

    return array(
      $fieldName => array(
        '#type' => 'value',
        '#title' => $this->getFieldTitle(),
        '#value' => $default,
      ),
    );
  }

  /**
   * Check to see if the field is enabled.
   *
   * @access public
   * @return boolean
   *	 TRUE if the field is enabled.
   */
  public function getFieldTitle() {
    return $this->getString('title');
  }

  /**
   * Check to see if the field is enabled.
   *
   * @access public
   * @return boolean
   *	 TRUE if the field is enabled.
   */
  public function isFieldEnabled() {
    return TRUE;
  }

  /**
   * Check to see if the field is required.
   *
   * @access public
   * @return boolean
   *	 TRUE if the field is required.
   */
  public function isFieldRequired() {
    return FALSE;
  }
}
