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
 * @param $arg1 The address object.
 * @param $arg2 If op = 'process', $arg2 is an array storing the
 *	values the user entered for the form.
 * @return If $op is 'new', 'add' or 'edit', return a form. If $op is
 * 	'process', return an address object. If $op is 'view', return
 *	an array of address fields to view.
 * @ingroup forms
 */
function uc_addresses_pane_address($op, $arg1, $arg2) {
  global $user;
  switch ($op) {
    case 'new':
    case 'add':
      $description = t('Enter your new address information.');
      $form['address'] = array(
	'#type' => 'fieldset',
	'#title' => t("Add an address"),
	'#collapsible' => TRUE,
	'#collapsed' => FALSE,
	);

      if (uc_address_field_enabled('first_name')) {
	$form['address']['first_name'] = uc_textfield(uc_get_field_name('first_name'), $arg1->first_name, uc_address_field_required('first_name'));
      }
      if (uc_address_field_enabled('last_name')) {
	$form['address']['last_name'] = uc_textfield(uc_get_field_name('last_name'), $arg1->last_name, uc_address_field_required('last_name'));
      }
      if (uc_address_field_enabled('company')) {
	$form['address']['company'] = uc_textfield(uc_get_field_name('company'), $arg1->company, uc_address_field_required('company'), NULL, 64);
      }
      if (uc_address_field_enabled('street1')) {
	$form['address']['street1'] = uc_textfield(uc_get_field_name('street1'), $arg1->street1, uc_address_field_required('street1'), NULL, 64);
      }
      if (uc_address_field_enabled('street2')) {
	$form['address']['street2'] = uc_textfield(uc_get_field_name('street2'), $arg1->street2, uc_address_field_required('street2'), NULL, 64);
      }
      if (uc_address_field_enabled('city')) {
	$form['address']['city'] = uc_textfield(uc_get_field_name('city'), $arg1->city, uc_address_field_required('city'));
      }
      if (uc_address_field_enabled('country')) {
	$form['address']['country'] = uc_country_select(uc_get_field_name('country'), $arg1->country, NULL, 'name', uc_address_field_required('country'));
      }
      if (uc_address_field_enabled('zone')) {
	if (isset($_POST['panes']['address']['country'])) {
	  $country_id = intval($_POST['panes']['address']['country']);
	}
	else {
	  $country_id = $arg1->country;
	}
	$form['address']['zone'] = uc_zone_select(uc_get_field_name('zone'), $arg1->zone, NULL, $country_id, 'name', uc_address_field_required('zone'));
      }
      if (uc_address_field_enabled('postal_code')) {
	$form['address']['postal_code'] = uc_textfield(uc_get_field_name('postal_code'), $arg1->postal_code, uc_address_field_required('postal_code'), NULL, 10, 10);
      }
      if (uc_address_field_enabled('phone')) {
	$form['address']['phone'] = uc_textfield(uc_get_field_name('phone'), $arg1->phone, uc_address_field_required('phone'), NULL, 32, 16);
      }
      if ($op == 'add') {
	$form['address']['save'] = array(
	  '#type' => 'checkbox',
	  '#title' => t('Default address'),
	  '#suffix' => t('Make this address the default address'),
	  '#default_value' => $arg1->is_default,
	  );
      }

      return array('description' => $description, 'contents' => $form['address'], 'theme' => 'addresses_pane');

    case 'edit':
      if ($arg1->is_default) {
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
	'#value' => $arg1->aid
	);
      if (uc_address_field_enabled('first_name')) {
	$form['address']['first_name'] = uc_textfield(uc_get_field_name('first_name'), $arg1->first_name, uc_address_field_required('first_name'));
      }
      if (uc_address_field_enabled('last_name')) {
	$form['address']['last_name'] = uc_textfield(uc_get_field_name('last_name'), $arg1->last_name, uc_address_field_required('last_name'));
      }
      if (uc_address_field_enabled('phone')) {
	$form['address']['phone'] = uc_textfield(uc_get_field_name('phone'), $arg1->phone, uc_address_field_required('phone'), NULL, 32, 16);
      }
      if (uc_address_field_enabled('company')) {
	$form['address']['company'] = uc_textfield(uc_get_field_name('company'), $arg1->company, uc_address_field_required('company'), NULL, 64);
      }
      if (uc_address_field_enabled('street1')) {
	$form['address']['street1'] = uc_textfield(uc_get_field_name('street1'), $arg1->street1, uc_address_field_required('street1'), NULL, 64);
      }
      if (uc_address_field_enabled('street2')) {
	$form['address']['street2'] = uc_textfield(uc_get_field_name('street2'), $arg1->street2, uc_address_field_required('street2'), NULL, 64);
      }
      if (uc_address_field_enabled('city')) {
	$form['address']['city'] = uc_textfield(uc_get_field_name('city'), $arg1->city, uc_address_field_required('city'));
      }
      if (uc_address_field_enabled('country')) {
	$form['address']['country'] = uc_country_select(uc_get_field_name('country'), $arg1->country, NULL, 'name', uc_address_field_required('country'));
      }
      if (uc_address_field_enabled('zone')) {
	if (isset($_POST['panes']['address']['country'])) {
	  $country_id = intval($_POST['panes']['address']['country']);
	}
	else {
	  $country_id = $arg1->country;
	}
	$form['address']['zone'] = uc_zone_select(uc_get_field_name('zone'), $arg1->zone, NULL, $country_id, 'name', uc_address_field_required('zone'));
      }
      if (uc_address_field_enabled('postal_code')) {
	$form['address']['postal_code'] = uc_textfield(uc_get_field_name('postal_code'), $arg1->postal_code, uc_address_field_required('postal_code'), NULL, 10, 10);
      }
      $form['address']['save'] = array(
	'#type' => 'checkbox',
	'#title' => t('Default address'),
	'#suffix' => t('Make this address the default address'),
	'#default_value' => $arg1->is_default,
	'#disabled' => $arg1->is_default,
	);
      return array('description' => $description, 'contents' => $form['address'], 'theme' => 'addresses_pane');

    case 'process':
      $arg1->aid = $arg2['aid'];
      $arg1->first_name = $arg2['first_name'];
      $arg1->last_name = $arg2['last_name'];
      $arg1->company = $arg2['company'];
      $arg1->street1 = $arg2['street1'];
      $arg1->street2 = $arg2['street2'];
      $arg1->city = $arg2['city'];
      $arg1->zone = $arg2['zone'];
      $arg1->postal_code = $arg2['postal_code'];
      $arg1->country = $arg2['country'];
      $arg1->phone = $arg2['phone'];
      $arg1->is_default = $arg2['save'];
      return TRUE;

    case 'view':
      $view[] = array('title' => t('Address'), 'data' => uc_addresses_address($arg1));
      if (uc_address_field_enabled('phone') && !empty($arg1->phone)) {
	$view[] = array('title' => t('Phone'), 'data' => check_plain($arg1->phone));
      }
      return $view;
  }
}

/**
 * Theme the address forms in tables.
 *
 * @param $form The address form to theme.
 * @return The themed form (as a string).
 */
function theme_addresses_pane($form) {
  $req = '<span class="form-required">*</span>';

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
    if (substr($field, 9) == 'street1') {
      $title = uc_get_field_name('street') .':';
    }
    elseif (substr($field, 9) == 'street2') {
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