<?php

/**
 * @file
 * Functions for Smileys module.
 */

/**
 * Helper function to import Adium Skype smileys.
 *
 * @see http://www.adiumxtras.com/index.php?a=xtras&xtra_id=2599
 */
function custom_skype_smileys_import() {
  // Clear out old smileys.
  db_delete('smileys')->execute();
  // @todo featurize or somehow save this.
  variable_set('smileys_path', 'sites/all/libraries/smileys');
  // Import ours.
  module_load_include('inc', 'smileys', 'smileys.admin');
  smileys_import();
  // Update their DB with info from Adium's xml manifest.
  $smileys = custom_parse_skype_smileys();
  foreach ($smileys as $smiley) {
    db_update('smileys')
      ->fields(array(
          'status' => 1,
          'acronyms' => $smiley['acronyms'],
          'description' => $smiley['description'],
        ))
      ->condition('uri', $smiley['uri'])
      ->execute();
  }
}

/**
 * Helper function to parse Adium Skype smileys Emoticons.plist.
 *
 * @see custom_skype_smileys_import()
 */
function custom_parse_skype_smileys() {
  $path = variable_get('smileys_path', 'sites/all/libraries/smileys');
  $folder = 'skypeallsmileys';
  $file_name = 'Emoticons.plist';
  // Get data.
  $data = file_get_contents($path . '/' . $folder . '/' . $file_name);
  // Parse the data.
  $xmlObj = simplexml_load_string($data);
  // @todo check out drupal_xml_parser_create() instead of this.
  $arrXml = objectsIntoArray($xmlObj);
  // Isolate what we want.
  $arr = $arrXml['dict']['dict'];

  // Build the output array.
  foreach ($arr['dict'] as $key => $smiley_info) {
    // This part of the array is either a string or an actual array of strings.
    $mixed = $smiley_info['array']['string'];
    $smileys[$key]['description'] = $smiley_info['string'];
    $smileys[$key]['acronyms'] = is_array($mixed) ? implode(' ', $mixed) : $mixed;
  }
  foreach ($arr['key'] as $key => $smiley_file) {
    $smileys[$key]['uri'] = $folder . '/' . $smiley_file;
  }

  return $smileys;
}

/**
 * Helper parse function for the Adium skype XML file.
 *
 * @see http://www.php.net/manual/en/function.xml-parse.php#97556
 */
function objectsIntoArray($arrObjData, $arrSkipIndices = array()) {
  $arrData = array();
  // if input is object, convert into array
  if (is_object($arrObjData)) {
    $arrObjData = get_object_vars($arrObjData);
  }
  if (is_array($arrObjData)) {
    foreach ($arrObjData as $index => $value) {
      if (is_object($value) || is_array($value)) {
        $value = objectsIntoArray($value, $arrSkipIndices); // recursive call
      }
      if (in_array($index, $arrSkipIndices)) {
        continue;
      }
      $arrData[$index] = $value;
    }
  }
  return $arrData;
}
