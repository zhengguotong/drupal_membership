<?php

function bv_membership_member_apply_page($form, &$form_state){
    global $base_url;

    $form = array();

    $form['#attached']['js'] = array(
        drupal_get_path('module', 'bv_membership') . '/js/jquery.cityselect.js',
    );

    $form['member'] = array(
        '#type' => 'fieldset',
        '#title' => t('申请会员'),
    );

    $form['member']['bank_name'] = array(
        '#type' => 'textfield',
        '#title' => t('银行名称'),
        '#size' => 60,
        '#required' => TRUE,
        '#maxlength' => 128,
    );

    $form['member']['bank_account'] = array(
        '#type' => 'textfield',
        '#title' => t('银行账号'),
         '#size' => 60,
        '#required' => TRUE,
        '#maxlength' => 128,
    );

    $form['member']['parent_phone'] = array(
        '#type' => 'textfield',
        '#title' => t('推荐人手机号码'),
        '#description' => t('若无推荐，请输入你的地址，我们会根据你的地址选择默认推荐人。'),
        '#size' => 60,
        '#maxlength' => 128,
        '#element_validate' => array('_bv_membership_china_phone_validation'),
    );

    $form['member']['city']= array( '#markup' => '<div class="city-select"><label class="control-label">数属地址</label><select class="form-control prov"></select>'
        . '<select class="form-control city" disabled="disabled"></select><select class="form-control dist" disabled="disabled"></select><select class="form-control town" disabled="disabled"></select></div>' );

    $form['member']['provh'] = array('#type' => 'hidden',
                                    '#default_value' => '' ,
                                    '#attributes' => array('class' => array('provh')));

    $form['member']['cityh'] = array('#type' => 'hidden',
                                    '#default_value' => '',
                                    '#attributes' => array('class' => array('cityh')));

    $form['member']['disth'] = array('#type' => 'hidden',
                                    '#default_value' => '',
                                    '#attributes' => array('class' => array('disth')));

    $form['member']['townh'] = array('#type' => 'hidden',
        '#default_value' => '',
        '#attributes' => array('class' => array('townh')));

    $form['member']['baseurlh'] = array('#type' => 'hidden',
                                        '#value' =>$base_url . '/' . drupal_get_path('module', 'bv_membership') . '/js/city.min.js',
                                        '#attributes' => array('class' => array('baseurlh')));

    $form['member']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('submit'),
         '#submit' => array(
            'bv_membership_member_apply_form_submit'
        )
    );

    return $form;
}

function bv_membership_member_apply_form_submit($form, &$formstate){
//    $edit['roles'][5] = '5';//auto assign 会员role
    global $user;
    $hasParentUser = false;
    $current_user = user_load($user->uid);


    if(!empty($formstate['values']['bank_name'])){
        $current_user->field_bank_name['und']['0']['value'] = $formstate['values']['bank_name'];
    }

    if(!empty($formstate['values']['bank_account'])){
        $current_user->field_bank_account['und']['0']['value'] = $formstate['values']['bank_account'];
    }

    if(!empty($formstate['values']['provh'])){
        $current_user->field_prev['und'][0]['value'] = $formstate['values']['provh'];
    }

    if(!empty($formstate['values']['cityh'])){
        $current_user->field_user_city['und'][0]['value'] = $formstate['values']['cityh'];
    }

    if(!empty($formstate['values']['disth'])){
        $current_user->field_user_dist['und'][0]['value'] = $formstate['values']['disth'];
    }

    if(!empty($formstate['values']['townh'])) {
         $current_user->field_user_town['und'][0]['value'] = $formstate['values']['townh'];
    }


    //如果推荐人手机号码有填写，查找是否有对应的用户
    if(!empty($formstate['values']['parent_phone'])){
        $current_user->field_parent_user_phone['und'][0]['value'] = $formstate['values']['parent_phone'];
        $uids = _bv_membership_get_user_by_phone($formstate['values']['parent_phone']);

        if(!empty($uids) && count($uids) > 0){
            $parent = user_load($uids[0]);
            if (in_array('会员', $parent->roles)){
                $current_user->field_parent_user['und'][0]['target_id'] = $parent->uid;
                $hasParentUser = true;
            }
        }
    }

    //如果还没有找到相应上家，根据输入的区级地址匹配默认上家
    if(!$hasParentUser){
        $stores = _bv_membership_users_by_role('加盟店');
        if(!empty($formstate['values']['townh'])){
            foreach ( $stores as $store) {
                if(!empty($store->field_user_town)){
                    if($store->field_user_town['und'][0]['value'] == $formstate['values']['townh']){
                        $current_user->field_parent_user['und'][0]['target_id'] = $store->uid;
                        $hasParentUser = true;
                        break;
                    }
                }
            }
        } else if(!empty($formstate['values']['disth'])){//如果区级不是空的
            foreach ( $stores as $store) {
              if(!empty($store->field_user_dist)){
                  if($store->field_user_dist['und'][0]['value'] == $formstate['values']['disth']){
                      $current_user->field_parent_user['und'][0]['target_id'] = $store->uid;
                      $hasParentUser = true;
                      break;
                  }
              }
            }
        }else if(!empty($formstate['values']['cityh'])){
            foreach ( $stores as $store) {
                if(!empty($store->field_user_city)){
                    if($store->field_user_city['und'][0]['value'] == $formstate['values']['cityh']){
                        $current_user->field_parent_user['und'][0]['target_id'] = $store->uid;
                        $hasParentUser = true;
                        break;
                    }
                }
            }
        }
    }
    
    user_save($current_user);
}

/**
 * Make sure parent phone number is in correct format
 * @param $element
 * @param $form_state
 */
function _bv_membership_china_phone_validation($element, &$form_state){
    if (!empty($element['#value'])&& !_bv_membership_valid_cn_phone_number($element['#value'])){
      form_error($element,  t('请输入正确的推荐人手机号码.'));
  }
}


/**
 * Verifies that $phonenumber is a valid Chinese phone number
 *
 * @param string $phonenumber
 * @return boolean Returns boolean FALSE if the phone number is not valid.
 */
function _bv_membership_valid_cn_phone_number($phonenumber) {

    $phonenumber = trim($phonenumber);

    // define regular expression
    $regex = '/^(\+86|86)?( |-)?([0-9]{11}|([0-9]{3,4}(\-|\.| )[0-9]{3,8})|[0-9]{2}( |\-)[0-9]{4}[ ][0-9]{4}|[0-9]{2}\.[0-9]{2}\.[0-9]{2}\.[0-9]{2}\.[0-9]{2})$/';

    // return true if valid, false otherwise
    return (bool) preg_match($regex, $phonenumber);
}

/**
 * Get user given by user phone
 * @param $phone string
 */
function _bv_membership_get_user_by_phone($phone){
    $uids = db_select('field_data_field_phone', 'pup')
        ->fields('pup', array('entity_id'))
        ->condition('pup.field_phone_value', $phone, '=')
        ->execute()
        ->fetchCol();
    return  $uids;
}

/**
 * get all user by role name
 * @param $role_name
 * @return mixed
 */
function _bv_membership_users_by_role($role_name) {
    $role = user_role_load_by_name($role_name);
    $uids = db_select('users_roles', 'ur')
        ->fields('ur', array('uid'))
        ->condition('ur.rid', $role->rid, '=')
        ->execute()
        ->fetchCol();
    $users = user_load_multiple($uids);
    return $users;
}
