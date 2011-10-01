/**
 * Javascript functions for Ubercart Addresses
 */

/**
 * Keeps track of the copy address checkbox being checked or not
 * @var boolean
 */
var uc_addresses_copy_box_checked = false;

/**
 * (Re)triggers the click event for the copy address checkbox
 * if this checkbox is checked
 */
$(document).ready(uc_addresses_trigger_copy_address);

/**
 * Copy address values from one pane to another
 *
 * @param boolean checkbox
 *   If the checkbox is checked or not
 * @param string source
 *   The pane to get the address data from
 *   The pane to copy address data to
 * @param string target
 *   The pane to copy address data to
 * @return void
 */
function uc_addresses_copy_address(checked, source, target) {
  var address_pane_source = '#' + source + '-pane';
  var address_pane_target = '#' + target + '-pane';

  if (checked) {
    // Copy over the zone options manually.
    zone_field_source = '#edit-panes-' + source + '-' + source + '-' + source + '-zone';
    zone_field_target = '#edit-panes-' + target + '-' + target + '-' + target + '-zone';
    if ($(zone_field_target).html() != $(zone_field_source).html()) {
      $(zone_field_target).empty().append($(zone_field_source).children().clone());
      $(zone_field_target).attr('disabled', $(zone_field_source).attr('disabled'));
    }

    // For each input field
    $(address_pane_target + ' input, select, textarea', ':visible', document.body).each(
      function(i) {
        // Copy the values from the source pane to the target pane
        var source_field = this.id;
        source_field = source_field.replace(target + '-' + target + '-' + target, source + '-' + source + '-' + source);
        var target_field = this.id;
        if (target_field != source_field) {
          if (this.type == 'checkbox') {
            $('#' + target_field).attr('checked', $('#' + source_field).attr('checked'));
          }
          else {
            $('#' + target_field).val($('#' + source_field).val());
          }
          $('#' + source_field).change(function () {uc_addresses_update_field(source_field, target_field); });
        }
      }
    );
    $(address_pane_target + ' div.address-pane-table').slideUp();
    uc_addresses_copy_box_checked = true;
  }
  else {
    $(address_pane_target + ' div.address-pane-table').slideDown();
    uc_addresses_copy_box_checked = false;
  }
}

/**
 * Copy a value from one pane to another when the source field has been changed.
 *
 * @param string source_field
 *   ID of the field that has been changed
 * @param string target_field
 *   ID of the field to update
 * @return void 
 */
function uc_addresses_update_field(source_field, target_field) {
  if (uc_addresses_copy_box_checked) {
    // Check if it is the zone field that changed.
    if (source_field.search('zone$') != -1) {
      if ($('#' + target_field).html() != $('#' + source_field).html()) {
        $('#' + target_field).empty().append($('#' + source_field).children().clone());
        $('#' + target_field).attr('disabled', $('#' + source_field).attr('disabled'));
      }
    }

    if ($('#' + source_field).attr('type') == 'checkbox') {
      $('#' + target_field).attr('checked', $('#' + source_field).attr('checked')).change();
    }
    else {
      source_value = $('#' + source_field).val();
      $('#' + target_field).val(source_value).change();
    }
  }
}

/**
 * (Re)triggers the click event for the copy address checkbox
 * if this checkbox is checked
 */
function uc_addresses_trigger_copy_address() {
  // Check if any copy address checkboxes are checked
  // and (re)trigger the click event
  $('.uc_addresses_copy_address').each(
    function() {
      // Make sure the html element is a checkbox
      if (this.type == 'checkbox') {
        if (this.checked == true) {
          // The click event must be triggered twice because the 
          // first trigger causes to checkbox to be unchecked!
          $(this).trigger('click');
          $(this).trigger('click');
        }
      }
    }
  );
}

/**
 * Apply the selected address to the appropriate fields in the cart form.
 */
function uc_addresses_apply_address(type, address_str) {
  if (address_str == '0') {
    return;
  }
   
  var address_pane = '#' + type + '-pane';
  var field_id_prefix = 'edit-panes-' + type + '-' + type + '-' + type + '-';

  eval('var address = ' + address_str + ';');

  $(address_pane + ' input, select, textarea', ':visible', document.body).each(
    function (i) {
      fieldname = this.id;
      fieldname = fieldname.replace(field_id_prefix, '');
      
      if (fieldname != 'country' && fieldname != 'zone' && address[fieldname] != undefined) {
        if (this.type == 'checkbox') {
          $(this).attr('checked', (address[fieldname] == 1)? true:false).trigger('change');
        }
        else {
          $(this).val(address[fieldname]).trigger('change');
        }
      }
    }
  );

  // Special treatment for country and zone fields
  if ($('#' + field_id_prefix + 'country').val() != address.country) {
    try {
      $('#' + field_id_prefix + 'country').val(address.country);
      uc_update_zone_select(field_id_prefix + 'country', address.zone);
    }
    catch (err) { }
  }
  $('#' + field_id_prefix + 'zone').val(address.zone).trigger('change');
}
