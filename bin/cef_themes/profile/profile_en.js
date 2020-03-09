window.onload = () => {
  const profile = NimCefWebInstance.profile

  $('#user-birthday-edit').datepicker({
    language: 'en-US',
    format: 'yyyy-mm-dd',
  })

  // WebRTC 开关的提示信息启用
  $('[data-toggle="popover"]').popover()
  $('[data-toggle="tooltip"]').tooltip()

  // Switch 开关启用，设置小按钮样式
  $('[data-toggle="toggle"]').bootstrapToggle({ size: 'small' })

  $('#edit-profile').click(() => {
    $('#user-details-preview').hide()
    $('#user-details-edit').show()
    $('#switch-group').hide();
  })

  $('#cancel').click(() => {
    $('#user-details-preview').show()
    $('#user-details-edit').hide()
    $('#switch-group').show();
  })

  // 通知 Native 程序弹出更换头像的窗口
  $('#user-avatar').click(() => {
    profile.updateHeadImage()
  })

  // 提交用户资料修改
  $('#submit').click(() => {
    const birthday = $('#user-birthday-edit').datepicker('getDate')
    const userInfo = Object.assign({}, {
      nickname: $('#user-nickname-edit').val(),
      gender: parseInt($('#user-gender-edit').val()),
      birthYear: birthday ? birthday.getFullYear() : 0,
      birthMonth: birthday ? birthday.getMonth() + 1 : 0,
      birthDay: birthday ? birthday.getDate() : 0,
      telephone: $('#user-telephone-edit').val(),
      email: $('#user-email-edit').val(),
      signature: $('#user-signature-edit').val()
    })

    profile.updateUserInfo(userInfo, (error, result) => {
      if (!error) {
        $('#user-details-preview').show()
        $('#user-details-edit').hide()
        $('#switch-group').show();
      } else {
        console.log(error, result)
      }
    })
  })

  // 注册一个方法，提供 native 端调用来设置用户信息
  profile.onUserInfoChanged((args) => {
    $('#user-avatar').attr('src', args.avatar || 'default_avatar.png')
  
    $('#user-nickname').text(args.nickname)
    $('#user-nickname-edit').val(args.nickname)
  
    $('#user-account').text(args.account)
    $('#user-account-edit').val(args.account)
  
    $('#user-birthday').text(args.birthday)
    $('#user-birthday-edit').datepicker('update', args.birthday);
  
    $('#user-telephone').text(args.telephone)
    $('#user-telephone-edit').val(args.telephone)
  
    $('#user-email').text(args.email)
    $('#user-email-edit').val(args.email)
  
    $('#user-signature').text(args.signature)
    $('#user-signature-edit').val(args.signature)
  
    if (args.gender === 1) {
      $('#user-gender').removeClass('fa-venus user-sex-female')
      $('#user-gender').addClass('fa-mars user-sex-male')
      $('#user-gender-edit').val('1')
    } else if (args.gender === 2) {
      $('#user-gender').removeClass('fa-mars user-sex-male')
      $('#user-gender').addClass('fa-venus user-sex-female')
      $('#user-gender-edit').val('2')
    }
  })

  // 注册一个方法，提供 native 端调用来设置多端推送开关
  profile.onMultiportPushChanged((args) => {
    const checked = $('#multiport-push').prop('checked')
    if (checked != args.multiportPush) {
      $('#multiport-push').bootstrapToggle(args.multiportPush ? 'on' : 'off')
    }
  })

  // 注册一个方法，提供 native 端调用来设置 WebRTC 开启状态
  profile.onWebRTCStatusChanged((args) => {
    const checked = $('#webrtc').prop('checked')
    if (checked != args.opening) {
      $('#webrtc').bootstrapToggle(args.opening ? 'on' : 'off')
    }
  })

  // 在首次设置完 WebRTC 状态后再开始监听按钮的切换状态
  $('#multiport-push').change(() => {
    const checked = $('#multiport-push').prop('checked')
    profile.updateMultiportPushStatus(checked)
  })

  // 在首次设置完 WebRTC 状态后再开始监听按钮的切换状态
  $('#webrtc').change(() => {
    const checked = $('#webrtc').prop('checked')
    profile.updateWebRTCStatus(checked)
  })
}
