<?php
/**
 * @file
 * Template for address form
 */
?>
<div class="address-pane-table">
  <table>
    <?php foreach (element_children($form) as $field): ?>
      <?php if ($form[$field]['#access'] !== FALSE): ?>
        <tr class="field-<?php print $field; ?>">
          <?php if ($form[$field]['#title']): ?>
            <td class="field-label">
              <?php if ($form[$field]['#required']): ?>
                <?php print $req; ?>
              <?php endif; ?>
              <?php print $form[$field]['#title']; ?>:
            </td>
          <?php unset($form[$field]['#title']); ?>
          <?php else: ?>
            <td class="field-label"></td>
          <?php endif; ?>
          <td class="field-field"><?php print drupal_render($form[$field]); ?></td>
        </tr>
      <?php endif; ?>
    <?php endforeach; ?>
  </table>
</div>
<div class="checkout-form-bottom"><?php print drupal_render($form); ?></div>