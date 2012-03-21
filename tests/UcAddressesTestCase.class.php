<?php
/**
 * @file
 * Contains base class for Ubercart Addresses tests.
 */

/**
 * Base class for Ubercart Addresses tests.
 */
abstract class UcAddressesTestCase extends DrupalWebTestCase {
  // -----------------------------------------------------------------------------
  // PROPERTIES
  // -----------------------------------------------------------------------------

  // Users

  /**
   * An account with no privileges, will be used to create addresses for by other users.
   */
  protected $basicUser;

  /**
   * Customer who may view, edit and delete own addresses.
   */
  protected $customer;

  /**
   * Admin user that has all the privileges of uc_addresses.
   */
  protected $adminUser;

  // -----------------------------------------------------------------------------
  // CONSTRUCT
  // -----------------------------------------------------------------------------

  /**
   * Install users and modules needed for all tests.
   *
   * @return void
   */
  public function setUp() {
    // Setup modules
    parent::setUp(
      'ctools',
      'token',
      'uc_store',
      'uc_addresses'
    );

    // Setup users
    $this->basicUser = $this->drupalCreateUser();
    $this->customer = $this->drupalCreateUser(array('add/edit own addresses', 'delete own addresses'));
    $this->adminUser = $this->drupalCreateUser(array('add/edit all addresses', 'delete all addresses'));

    // Revoke default permissions for authenticated user, so we can test the effect of permissions.
    user_role_revoke_permissions(DRUPAL_AUTHENTICATED_RID, array(
        'view own addresses',
        'add/edit own addresses',
        'delete own addresses',
      )
    );
  }

  // -----------------------------------------------------------------------------
  // ADDRESS BOOK CRUD
  // -----------------------------------------------------------------------------

  /**
   * View the address book of an user.
   *
   * @param object $account
   *   The user to view the address book for.
   * @param boolean $may_view
   *   (optional) If expected if the user may view the address.
   *
   * @return void
   */
  protected function viewAddressBook($account, $may_view = NULL) {
    // Go to the address book
    $this->drupalGet($this->constructAddressUrl($account));
    // Test response code
    if (!is_null($may_view)) {
      $this->assertResponse(($may_view) ? 200 : 403);
    }
  }

  /**
   * View a single address of an user.
   *
   * @param object $account
   *   The user to view an address for
   * @param int $aid
   *   The ID of the address to view
   * @param boolean $may_view
   *   (optional) If expected if the user may view the address.
   *
   * @return void
   */
  protected function viewAddress($account, $aid, $may_view = NULL) {
    // Go to the view address page
    $this->drupalGet($this->constructAddressUrl($account, $aid));
    // Test response code
    if (!is_null($may_view)) {
      $this->assertResponse(($may_view) ? 200 : 403);
    }
  }

  /**
   * Create a new address for an user.
   *
   * @param object $account
   *   The user to create an address for
   * @param boolean $may_edit
   *   If expected if the user may edit the address.
   *   Defaults to TRUE
   * @param array $values
   *   (optional) The values for the address to set
   *   If not given, default values will be used for the address.
   *
   * @return int
   *   The address ID for the created address if creating was succesful.
   *   NULL Otherwise.
   * @todo Return the address ID.
   */
  protected function createAddress($account, $may_edit = TRUE, $values = array()) {
    if ($may_edit) {
      $values = $this->getEditAddressValues(array('address'), $values, 'address_form');
      $this->drupalPost($this->constructAddressUrl($account) . 'add', $values['form_values'], t('Save address'));
      $this->assertText(t('The address is saved.'), t('The address was saved.'));

      // Lookup address to find out ID.
      $aid = db_query("SELECT aid FROM {uc_addresses}
      WHERE uid = :uid
      ORDER BY aid DESC
      ", array(':uid' => $account->uid))->fetchField();

      // Ensure any given values exists based on whether they should be displayed.
      $this->viewAddress($account, $aid);
      $values['values']['aid'] = $aid;
      $this->doAddressValuesDisplayedTests($values['values'], 'address_view');
      $this->assertTrue($this->checkAddressValuesInDatabase($values['values']), t('The address %aid is correctly saved to the database.', array('%aid' => $aid)));

      return $aid;
    }
    else {
      $this->drupalGet($this->constructAddressUrl($account) . 'add');
      $this->assertResponse(403);
    }
    return NULL;
  }

  /**
   * Edit an address of an user.
   *
   * @param object $account
   *   The user whose address must be edited
   * @param int $aid
   *   The ID of the address to edit
   * @param boolean $may_edit
   *   If expected if the user may edit the address.
   *   Defaults to TRUE
   * @param array $values
   *   (optional) The values for the address to set
   *   If not given, default values will be used for the address.
   *
   * @return void
   */
  protected function editAddress($account, $aid, $may_edit = TRUE, $values = array()) {
    if ($may_edit) {
      $values = $this->getEditAddressValues(array('address'), $values, 'address_form');
      $this->drupalPost($this->constructAddressUrl($account, $aid) . 'edit', $values['form_values'], t('Save address'));
      $this->assertText(t('The address is saved.'), t('The address was saved.'));

      // Ensure any given values exists based on whether they should be displayed.
      $this->viewAddress($account, $aid);
      $values['values']['aid'] = $aid;
      $this->doAddressValuesDisplayedTests($values['values'], 'address_view');
      $this->assertTrue($this->checkAddressValuesInDatabase($values['values']), t('The address %aid is correctly saved to the database.', array('%aid' => $aid)));
    }
    else {
      $this->drupalGet($this->constructAddressUrl($account, $aid) . 'edit');
      $this->assertResponse(403);
    }
  }

  /**
   * Delete an address of an user.
   *
   * @param object $account
   *   The user whose address must be deleted
   * @param int $aid
   *   The ID of the address to delete
   * @param boolean $may_delete
   *   If expected if the user may delete the address.
   *   Defaults to TRUE.
   *
   * @return void
   * @todo Assert text
   */
  protected function deleteAddress($account, $aid, $may_delete = TRUE) {
    if ($may_delete) {
      $this->drupalPost($this->constructAddressUrl($account, $aid) . 'delete', array(), t('Delete address'));
      $this->assertText(t('The address has been deleted.'), t('Deleting address succesful'));
    }
    else {
      $this->drupalGet($this->constructAddressUrl($account, $aid) . 'delete');
      $this->assertResponse(403);
    }
  }

  /**
   * Test if user can view, edit or delete the address.
   *
   * @param object $account
   *   The account to view the address for.
   * @param int $aid
   *   The ID of the address to do crud tests for.
   * @param boolean $may_view
   *   If the address may be viewed by the currently logged in user.
   * @param boolean $may_edit
   *   If the address may be viewed by the currently logged in user.
   * @param boolean $may_delete
   *   If the address may be viewed by the currently logged in user.
   * @param array $values
   *   (optional) Values to fill in when editing the address.
   *
   * @return void
   */
  protected function doCrudTests($account, $aid, $may_view = TRUE, $may_edit = TRUE, $may_delete = TRUE, $values = array()) {
    // View this address
    $this->viewAddress($account, $aid, $may_view);

    // Edit this address
    $this->editAddress($account, $aid, $may_edit, $values);

    // Delete this address
    $this->deleteAddress($account, $aid, $may_delete);
  }

  /**
   * Test if these address values are displayed on the page.
   *
   * @param array $values
   *   The values that should exist
   * @param string $context
   *   (optional) The context in which the address is displayed.
   * @param string $output
   *   (optional) The output to test.
   *   Defaults to the output in the browser
   *
   * @return void
   */
  protected function doAddressValuesDisplayedTests($values, $context = 'default', $output = NULL) {
    $address = UcAddressesAddressBook::newAddress();
    $handlers = uc_addresses_get_address_field_handler_instances($address, $context);
    foreach ($handlers as $fieldname => $handler) {
      if (!isset($values[$fieldname])) {
        continue;
      }
      // Check if field is used in the context and if it is enabled.
      if ($handler->checkContext() && $handler->isFieldEnabled()) {
        $value = $handler->outputValue($values[$fieldname]);
        if (drupal_strlen($value) > 0) {
          if ($output) {
            $this->assertTrue((strpos($output, $value) !== FALSE), t('Value %value found for field %field on the page.', array('%value' => $value, '%field' => $fieldname)));
          }
          else {
            $this->assertText($value, t('Value %value found for field %field on the page.', array('%value' => $value, '%field' => $fieldname)));
          }
        }
      }
    }
  }

  /**
   * Test if these address values appear in the database.
   *
   * @param array $values
   *
   * @return boolean
   */
  protected function checkAddressValuesInDatabase($values, $context = 'default') {
    $schema_values = array();

    // Only check real schema fields
    foreach ($values as $fieldname => $value) {
      if (UcAddressesSchemaAddress::schemaFieldExists($fieldname)) {
        $schema_values[$fieldname] = $value;
      }
    }

    $query = db_select('uc_addresses');
    $query->addExpression('COUNT(aid)');
    foreach ($schema_values as $fieldname => $value) {
      $query->condition($fieldname, $value);
    }
    $sQuery = (string) $query;
    $result = $query->execute();

    return ($result->fetchField()) ? TRUE : FALSE;
  }

  // -----------------------------------------------------------------------------
  // HELPER FUNCTIONS
  // -----------------------------------------------------------------------------

  /**
   * Constructs the url for the address book.
   *
   * If any paths change in the future we only need to change it here.
   *
   * @param object $account
   *   The user whose address must be deleted
   * @param int $aid
   *   (optional) The ID of the address to delete
   *   Defaults to NULL
   *
   * @return string
   */
  protected function constructAddressUrl($account, $aid = NULL) {
    $url = 'user/' . $account->uid . '/addresses/';
    if ($aid) {
      $url .= $aid . '/';
    }
    return $url;
  }

  /**
   * Generates an array of values to post into an address form
   *
   * @param array $parents
   *   The parent form elements.
   * @param array $values
   *   (Some of) the values to use in the address form.
   * @param string $context
   *   The context of the address form.
   *   This is to determine which address fields should be available.
   * @param string $prefix
   *   Optionally prefixes every field name
   *
   * @return array
   *   An array with for each field a value.
   *
   * @todo Think of which values go in the values array.
   */
  protected function getEditAddressValues($parents = array(), $values = array(), $context = 'default', $prefix = '') {
    // Initialize values array
    $form_values = array();

    // Calculate parent string if needed.
    $parent_string = '';
    if (count($parents) > 0) {
      foreach ($parents as $parent) {
        if ($parent_string) {
          $parent_string = $parent_string . '[' . $parent . ']';
        }
        else {
          $parent_string = $parent;
        }
      }
    }

    $address = UcAddressesAddressBook::newAddress();
    $handlers = uc_addresses_get_address_field_handler_instances($address, $context);
    foreach ($handlers as $fieldname => $handler) {
      if ($handler instanceof UcAddressesDefaultAddressFieldHandler) {
        // Bypass fill in values for marking it as the default.
        continue;
      }
      if (isset($values[$fieldname])) {
        // The value is already set. Do not override it.
        continue;
      }
      // Check if field is used in the context
      if ($handler->checkContext()) {
        // Fill in a value.
        $values[$fieldname] = $this->generateAddressFieldValue($fieldname, $values);
      }
    }

    // Prefix values and add parents
    foreach ($values as $fieldname => $value) {
      // Set in parents if needed
      $formfieldname = $prefix . $fieldname;
      if ($parent_string) {
        $formfieldname = $parent_string . '[' . $formfieldname . ']';
      }
      $form_values[$formfieldname] = $value;
    }

    return array(
      'form_values' => $form_values,
      'values' => $values,
    );
  }

  /**
   * Generates a value for an address field.
   *
   * @param string $fieldname
   *   The field to generate a value for
   * @param array $values
   *   The field values already generated
   *
   * @return string
   *   The generated value
   */
  protected function generateAddressFieldValue($fieldname, &$values) {
    switch ($fieldname) {
      case 'address_name':
        // By default an empty address name is returned to avoid name collisions
        // when that aspect is not tested.
        return '';
      case 'default_shipping':
      case 'default_billing':
        // Don't make addresses the default if this aspect is not tested.
        return 0;
      case 'postal_code':
        // A numeric code for postal codes
        return mt_rand(10000, 99999);
      case 'country':
        // The default country in Ubercart
        return variable_get('uc_store_country', 840);
      case 'zone':
        // Random zone based on the defined country
        if (!isset($values['country'])) {
          $values['country'] = $this->generateAddressFieldValue('country', &$values);
        }
        return db_query("SELECT zone_id FROM {uc_zones} WHERE zone_country_id = :zone_country_id ORDER BY rand()", array(':zone_country_id' => $values['country']))->fetchField();
      default:
        // In all other cases it is assummed that it's a textfield that needs to be filled in.
        return self::randomString(12);
    }
  }
}
