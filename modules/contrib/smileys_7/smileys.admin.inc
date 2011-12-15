<?php
/**
 * @file
 *   Smileys administrative pages
 */


/**
 * Smileys overview form
 */
function smileys_overview_form($form, &$form_state) {

  $header = array(
    array('data' => t('Picture'), ),
    array('data' => t('File'), 'field' => 'uri'),
    array('data' => t('Enabled'), 'field' => 'status', 'sort' => 'desc'),
    array('data' => t('Acronyms'), 'field' => 'acronyms'),
    array('data' => t('Description'), 'field' => 'description'),
    array('data' => t('Delete')),
  );
  $query = db_select('smileys', 'em')->extend('PagerDefault')->extend('TableSort');;
  $query->fields('em');
  $result = $query
    ->orderByHeader($header)
    ->limit(15)
    ->execute();

  $smileys_path = drupal_get_path('module', 'smileys');
  foreach ($result AS $smiley) {
    $form += smileys_overview_item($smiley);
  }

  // Fields for a new smiley
  $smiley = new stdClass;
  $smiley->sid = 'new';
  $smiley->uri = '';
  $smiley->status = 0;
  $smiley->acronyms = '';
  $smiley->description = '';
  $form += smileys_overview_item($smiley);

  $form['new']['delete'] = NULL;
  $form['new']['picture']['#markup'] = 'New';
  $form['new']['uri']['#required'] = FALSE;

  $form['pager'] = array('#theme' => 'pager');
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  $form['#header'] = $header;
  return $form;
}


/**
 * Helper function for smileys_overview_form()
 */
function smileys_overview_item($smiley) {
  static $smileys_path;
  if ($smileys_path == NULL) {
    $smileys_path = variable_get('smileys_path', drupal_get_path('module', 'smileys') . '/packs');
  }
  $img_vars = array(
    'path' => $smileys_path . '/' . $smiley->uri,
    'alt' => 'smiley',
    'attributes' => array(),
  );
  $form[$smiley->sid]['picture']['#markup'] = theme_image($img_vars);
  $form[$smiley->sid]['uri'] = array(
    '#type' => 'textfield',
    '#default_value' => $smiley->uri,
    '#required' => TRUE,
    '#size' => 25,
    '#title' => t('File'),
    '#title_display' => 'invisible',
  );
  $form[$smiley->sid]['status'] = array(
    '#type' => 'checkbox',
    '#default_value' => $smiley->status,
  );
  $form[$smiley->sid]['acronyms'] = array(
    '#type' => 'textfield',
    '#default_value' => $smiley->acronyms,
    '#size' => 50,
  );
  $form[$smiley->sid]['description'] = array(
    '#type' => 'textfield',
    '#default_value' => $smiley->description,
    '#size' => 10,
  );
  $form[$smiley->sid]['delete'] = array(
    '#type' => 'checkbox',
    '#default_value' => 0,
  );
  $form[$smiley->sid]['#tree'] = TRUE;
  $form[$smiley->sid]['#smiley'] = TRUE;

  return $form;
}


/**
 * Smileys overview form validate
 */
function smileys_overview_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  foreach ($values AS $sid => $value) {
    if (is_array($value) && !empty($value['uri']) && empty($value['delete'])) {
      $exist = db_query('SELECT COUNT(*) FROM {smileys} WHERE uri=? AND sid<>?', array($value['uri'], $sid))->fetchColumn();
      if ($exist) {
        form_set_error($sid . '][uri', t('This uri already exists'));
      }
      elseif (!preg_match('#^[a-z0-9_\-/]*\.(gif|png|jpg)$#i', $value['uri'])) {
        form_set_error($sid . '][uri', t('Incorrect uri.'));
      }
    }
  }
}


/**
 * Smileys form submit
 */
function smileys_overview_form_submit($form, &$form_state) {
  $values = $form_state['values'];

  $delete = array();
  $update_cnt = 0;
  foreach ($values AS $sid => $value) {
    if (is_array($value) && !empty($value['uri'])) {
      $smiley = (object) $value;
      $smiley->sid = $sid;
      $smiley->description = strip_tags($smiley->description);

      // Adding
      if ($sid == 'new') {
        drupal_write_record('smileys', $smiley);
        drupal_set_message(t('New smiley has been added.'));
      }
      // Deleting
      elseif ($value['delete']) {
        $delete[] = $sid;
      }
      // Updateting
      else {
        $update_cnt++;
        drupal_write_record('smileys', $smiley, 'sid');
      }
    }
  }
  if ($update_cnt) {
    drupal_set_message(format_plural($update_cnt, 'Updated 1 smiley.', '@count smileys has been updated'));
  }
  // Deleting
  if ($delete) {
    $num_deleted = db_delete('smileys')
      ->condition('sid', $delete, 'IN')
      ->execute();
    drupal_set_message(format_plural($num_deleted, 'Deleted 1 smiley.', '@count smileys has been deleted'));
  }
}


/**
 * Returns HTML for the smileys overview form into a table.
 *
 * @param $vars
 *   An associative array containing:
 *   - form: A render element representing the form.
 *
 * @ingroup themeable
 */
function theme_smileys_overview_form($vars) {
  $form = $vars['form'];
  foreach (element_children($form) as $smid) {
    $element = &$form[$smid];
    if (empty($element['#smiley'])) {
      continue;
    }
    $row = array();
    foreach ($element AS $k => $v) {
      if ($k[0] != '#') {
        $row[] = array('data' => drupal_render($element[$k]), 'style' => 'text-align: center');
      }
    }
    $row = array_merge(array('data' => $row), $element['#attributes']);
    $rows[] = $row;
  }
  $output = theme('table', array('header' => $form['#header'], 'rows' => $rows, 'attributes' => array('id' => 'smileys-overview')));
  $output .= drupal_render_children($form);
  return $output;
}


/**
 * Settings form
 */
function smileys_settings_form($form, &$form_state) {
  $form['note'] = array(
    '#markup' => t('It is the best practice to hold your smileys in separate folder under website\'s public files or resources directory.'),
  );
  $form['smileys_path'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => 'Path to smileys packs directory',
    '#maxlength' => 256,
    '#default_value' => variable_get('smileys_path', drupal_get_path('module', 'smileys') . '/packs'),
    '#size' => 64,
  );
  return system_settings_form($form);
}


/**
 * Settings form validate
 */
function smileys_settings_form_validate($form, &$form_state) {
  if (!is_dir($form_state['values']['smileys_path'])) {
    form_set_error('smileys_path', t('Directory does not exist'));
  }
}


/**
 * Smileys import form
 */
function smileys_import_form($form, &$form_state) {
  $form['help'] = array(
    '#prefix' => '<p>',
    '#markup' => t('Before import new smileys, download smiley packs (!kolobok, !phpBB or other) and extract them in smileys folder (<strong>@folder</strong>).', array(
      '!kolobok' => l(t('kolobok'), 'http://www.en.kolobok.us/download.php?list.8', array('attributes' => array('title ' => 'Author\'s smileys Kolobok Style'))),
      '!phpBB'   => l(t('phpBB'), 'http://www.phpbb.com/customise/db/styles/smilies-13/', array('attributes' => array('title ' => 'Smyles for phpBB'))),
      '@folder'  => variable_get('smileys_path', drupal_get_path('module', 'smileys') . '/packs'),
    )),
    '#suffix' => '</p>',
  );
  $form['status'] = array(
    '#title' => t('Enable all new smileys'),
    '#type' => 'checkbox',
    '#default_value' => 0,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Start import'),
  );
  return $form;
}


/**
 * Smileys import form submit
 */
function smileys_import_form_submit($form, &$form_state) {
  $num_added = smileys_import($form_state['values']['status']);
  if ($num_added) {
    drupal_set_message(format_plural($num_added, '1 smiley has been added', '@count smileys has been added'));
  }
  else {
    drupal_set_message(t('There is no new smiles in directory <em>@folder</em>',
    array('@folder' => variable_get('smileys_path', drupal_get_path('module', 'smileys') . '/packs'))), 'warning');
  }
}


/**
 * Import smileys
 *
 * @param $status
 *   (optinal) Status of new smileys.
 *
 * @return
 *   Number of added smileys
 */
function smileys_import($status = 0) {
  $packs_path = variable_get('smileys_path', drupal_get_path('module', 'smileys') . '/packs');
  $files = file_scan_directory($packs_path, '#.\.(gif|png|jpg)$#i');
  $query = db_insert('smileys')->fields(array('uri', 'status', 'acronyms', 'description'));
  $total = 0;
  foreach ($files AS $file) {
    $values = array(
      'uri' => substr($file->uri, drupal_strlen($packs_path) + 1),
      'status' => $status,
      'acronyms' => '*' . $file->name . '*',
      // "yes3" will be "Yes 3"
      'description' => drupal_ucfirst(preg_replace(array('/_/', '/(\d+)$/'), array(' ', ' $1'), $file->name)),
    );
    $exist = db_query('SELECT COUNT(*) FROM {smileys} WHERE uri = ?', array($values['uri']))->fetchColumn();
    if (!$exist) {
      $query->values($values);
      $total++;
    }
  }
  $query->execute();
  return $total;
}
