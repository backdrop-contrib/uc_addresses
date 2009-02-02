<?php
  // $Id$

/**
 * @file
 * This file contains the callbacks for the default address panes and their
 * corresponding helper functions.
 *
 * Address panes are defined using hook_address_pane() and use a callback to handle
 * the different processes involved in address viewing/editing.  The default address
 * panes are defined in uc_addresses_address_pane() in
 * uc_addresses.module.
 *
 * @ingroup uc_addresses
 */

/**
 * Create, edit, process or view the Address pane.
 *
 * @param $op The name of the operation to perform. One of 'new',
 *	'add, 'edit', 'process' or 'view'.
 * @param $address The address object.
 * @param $form_values If op = 'process', this is an array storing the
 *	values the user entered for the form.
 * @return If $op is 'new', 'add' or 'edit', return a form. If $op is
 * 	'process', return an address object. If $op is 'view', return
 *	an array of address fields to view.
 * @ingroup forms
 */
function uc_addresses_pane_address($op, $address, $form_values) {
  global $user;

  switch ($op) {
    case 'new':
    case 'add':
      $description = t('Enter your new address information.');
      $form['address'] = array(
	'#type' => 'fieldset',
	'#title' => t('Add an address'),
	'#collapsible' => TRUE,
	'#collapsed' => FALSE,
	);

      if (!$address) {
	$address = uc_addresses_blank_address();
      }

      if (uc_address_field_enabled('first_name')) {
	$form['address']['first_name'] = uc_textfield(uc_get_field_name('first_name'), $address->first_name, uc_address_field_required('first_name'));
      }
      if (uc_address_field_enabled('last_name')) {
	$form['address']['last_name'] = uc_textfield(uc_get_field_name('last_name'), $address->last_name, uc_address_field_required('last_name'));
      }
      if (uc_address_field_enabled('company')) {
	$form['address']['company'] = uc_textfield(uc_get_field_name('company'), $address->company, uc_address_field_required('company'), NULL, 64);
      }
      if (uc_address_field_enabled('street1')) {
	$form['address']['street1'] = uc_textfield(uc_get_field_name('street1'), $address->street1, uc_address_field_required('street1'), NULL, 64);
      }
      if (uc_address_field_enabled('street2')) {
	$form['address']['street2'] = uc_textfield(uc_get_field_name('street2'), $address->street2, uc_address_field_required('street2'), NULL, 64);
      }
      if (uc_address_field_enabled('city')) {
	$form['address']['city'] = uc_textfield(uc_get_field_name('city'), $address->city, uc_address_field_required('city'));
      }
      if (uc_address_field_enabled('country')) {
	$form['address']['country'] = uc_country_select(uc_get_field_name('country'), $address->country, NULL, 'name', uc_address_field_required('country'));
      }
      if (uc_address_field_enabled('zone')) {
	$country_id = $address->country;
	if ($op == 'add') {
	  if (isset($_POST['panes'], $_POST['panes']['address'], $_POST['panes']['address']['country'])) {
	    $country_id = intval($_POST['panes']['address']['country']);
	  }
	}
	else {
	  if (isset($_POST['country'])) {
	    $country_id = intval($_POST['country']);
	  }
        }

	$form['address']['zone'] = uc_zone_select(uc_get_field_name('zone'), $address->zone, NULL, $country_id, 'name', uc_address_field_required('zone'));
      }
      if (uc_address_field_enabled('postal_code')) {
	$form['address']['postal_code'] = uc_textfield(uc_get_field_name('postal_code'), $address->postal_code, uc_address_field_required('postal_code'), NULL, 10, 10);
      }
      if (uc_address_field_enabled('phone')) {
	$form['address']['phone'] = uc_textfield(uc_get_field_name('phone'), $address->phone, uc_address_field_required('phone'), NULL, 32, 16);
      }
      if ($op == 'add') {
	$form['address']['address_name'] = uc_textfield(t('Save as'), $address->address_name, FALSE, t('Enter a name for this address (e.g. Home or Work)'));

	$form['address']['save'] = array(
	  '#type' => 'checkbox',
	  '#title' => t('Default address'),
	  '#suffix' => t('Make this address the default address'),
	  '#default_value' => $address->is_default,
	  );
      }

      drupal_add_js(drupal_get_path('module', 'uc_addresses') .'/uc_addresses.js');

      return array('description' => $description, 'contents' => $form['address'], 'theme' => 'uc_addresses_pane');

    case 'edit':
      if ($address->is_default) {
	$description = t('Edit your default address information.');
      }
      else {
	$description = t('Edit your address information.');
      }
      $form['address'] = array(
	'#type' => 'fieldset',
	'#title' => t("Edit address information"),
	'#collapsible' => TRUE,
	'#collapsed' => FALSE,
	);
      $form['address']['aid'] = array(
	'#type' => 'hidden',
	'#value' => $address->aid
	);
      if (uc_address_field_enabled('first_name')) {
	$form['address']['first_name'] = uc_textfield(uc_get_field_name('first_name'), $address->first_name, uc_address_field_required('first_name'));
      }
      if (uc_address_field_enabled('last_name')) {
	$form['address']['last_name'] = uc_textfield(uc_get_field_name('last_name'), $address->last_name, uc_address_field_required('last_name'));
      }
      if (uc_address_field_enabled('phone')) {
	$form['address']['phone'] = uc_textfield(uc_get_field_name('phone'), $address->phone, uc_address_field_required('phone'), NULL, 32, 16);
      }
      if (uc_address_field_enabled('company')) {
	$form['address']['company'] = uc_textfield(uc_get_field_name('company'), $address->company, uc_address_field_required('company'), NULL, 64);
      }
      if (uc_address_field_enabled('street1')) {
	$form['address']['street1'] = uc_textfield(uc_get_field_name('street1'), $address->street1, uc_address_field_required('street1'), NULL, 64);
      }
      if (uc_address_field_enabled('street2')) {
	$form['address']['street2'] = uc_textfield(uc_get_field_name('street2'), $address->street2, uc_address_field_required('street2'), NULL, 64);
      }
      if (uc_address_field_enabled('city')) {
	$form['address']['city'] = uc_textfield(uc_get_field_name('city'), $address->city, uc_address_field_required('city'));
      }
      if (uc_address_field_enabled('country')) {
	$form['address']['country'] = uc_country_select(uc_get_field_name('country'), $address->country, NULL, 'name', uc_address_field_required('country'));
      }
      if (uc_address_field_enabled('zone')) {
	if (isset($_POST['panes'], $_POST['panes']['address'], $_POST['panes']['address']['country'])) {
	  $country_id = intval($_POST['panes']['address']['country']);
	}
	else {
	  $country_id = $address->country;
	}
	$form['address']['zone'] = uc_zone_select(uc_get_field_name('zone'), $address->zone, NULL, $country_id, 'name', uc_address_field_required('zone'));
      }
      if (uc_address_field_enabled('postal_code')) {
	$form['address']['postal_code'] = uc_textfield(uc_get_field_name('postal_code'), $address->postal_code, uc_address_field_required('postal_code'), NULL, 10, 10);
      }
      $form['address']['address_name'] = uc_textfield(t('Save as'), $address->address_name, FALSE, t('Enter a name for this address (e.g. Home or Work)'));
      $form['address']['save'] = array(
	'#type' => 'checkbox',
	'#title' => t('Default address'),
	'#suffix' => t('Make this address the default address'),
	'#default_value' => $address->is_default,
	'#disabled' => $address->is_default,
	);

      drupal_add_js(drupal_get_path('module', 'uc_addresses') .'/uc_addresses.js');

      return array('description' => $description, 'contents' => $form['address'], 'theme' => 'uc_addresses_pane');

    case 'process':
      $address->first_name = $form_values['first_name'];
      $address->last_name = $form_values['last_name'];
      $address->company = $form_values['company'];
      $address->street1 = $form_values['street1'];
      $address->street2 = $form_values['street2'];
      $address->city = $form_values['city'];
      $address->zone = $form_values['zone'];
      $address->postal_code = $form_values['postal_code'];
      $address->country = $form_values['country'];
      $address->phone = $form_values['phone'];
      $address->address_name = $form_values['address_name'];
      $address->is_default = $form_values['save'];
      return TRUE;

    case 'view':
      $view[] = array('title' => t('Address'), 'data' => uc_addresses_address($address));
      if (uc_address_field_enabled('phone') && !empty($address->phone)) {
	$view[] = array('title' => t('Phone'), 'data' => check_plain($address->phone));
      }
      return $view;
  }
}

/**
 * Create a blank address. This is the address we use when first
 * displaying a form used to add an address.
 *
 * @return A blank address object
 */
function uc_addresses_blank_address()
{
  $address = new stdClass();
  $address->first_name = '';
  $address->last_name = '';
  $address->company = '';
  $address->street1 = '';
  $address->street2 = '';
  $address->city = '';
  $address->zone = '';
  $address->postal_code = '';
  $address->country = '';
  $address->phone = '';
  $address->address_name = '';
  $address->is_default = false;
  return $address;
}

/**
 * Theme the address forms in tables.
 *
 * @param $form The address form to theme.
 * @return The themed form (as a string).
 */
function theme_uc_addresses_pane($form) {
  $req = '<span class="form-required">*</span>';

  $output = '';
  if (isset($form['copy_address'])) {
    $output = drupal_render($form['copy_address']);
  }

  $output .= '<div class="address-pane-table"><table>';

  foreach (element_children($form) as $field) {
    if ($field == 'aid') {
      continue;
    }
    $title = $form[$field]['#title'] .':';
    unset($form[$field]['#title']);
    if (substr($field, -7) == 'street1') {
      $title = uc_get_field_name('street') .':';
    }
    elseif (substr($field, -7) == 'street2') {
      $title = ' ';
    }
    $output .= '<tr><td class="field-label">';
    if ($form[$field]['#required']) {
      $output .= $req;
    }
    $output .= $title .'</td><td>'. drupal_render($form[$field]) .'</td></tr>';
  }
  $output .= '</table></div>';

  foreach (element_children($form) as $element) {
    $output .= drupal_render($form[$element]);
  }

  return $output;
}

/**
 * Build a list of address panes defined in the enabled modules.
 *
 * @param $view ???
 * @return A sorted array of panes.
 */
function _address_pane_list($view = 'view') {
  static $panes;

  $panes = module_invoke_all('address_pane', NULL);
  for ($i = 0; $i < count($panes); $i++) {
    $panes[$i]['enabled'] = variable_get('uc_addresses_address_pane_'. $panes[$i]['id'] .'_enabled', (!isset($panes[$i]['enabled']) ? TRUE : $panes[$i]['enabled']));
    $panes[$i]['weight'] = variable_get('uc_addresses_address_pane_'. $panes[$i]['id'] .'_weight_'. $view, (!isset($panes[$i]['weight']) ? 0 : $panes[$i]['weight']));
  }
  usort($panes, 'uc_weight_sort');

  return $panes;
}

/**
 * Return data from an address pane by pane ID and the array key.
 *
 * @param $pane_id The id of the pane.
 * @param $key The key used to access a value in the pane.
 * @return The requested value or NULL if the key doesn't exist.
 */
function _address_pane_data($pane_id, $key) {
  $panes = _address_pane_list();
  foreach ($panes as $pane) {
    if ($pane['id'] == $pane_id) {
      return $pane[$key];
    }
  }
}