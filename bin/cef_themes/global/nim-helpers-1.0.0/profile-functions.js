((obj) => {
  const g = {}

  /********************************************************************************
   * 更新用户资料
   */
  g.updateUserInfo = (profile, callback) => {
    obj.call('updateUserInfo', profile, typeof callback === 'function' ? callback : (error, result) => {
      if (error) {
        console.log(result)
      }
    })
  }

  /**
   * 向 native 程序发起弹出更换头像窗口的请求
   */
  g.updateHeadImage = (callback) => {
    NimCefWebInstance.call('updateHeadImage', typeof callback === 'function' ? callback : (error, result) => {
      if (error) {
        console.log(error, result)
      }
    })
  }

  /**
   * 设置多端免打扰开关
   */
  g.updateMultiportPushStatus = (multiportPush, callback) => {
    NimCefWebInstance.call('updateMultiportPushStatus', { multiportPush }, typeof callback === 'function' ? callback : (error, result) => {
      if (error) {
        console.log(error, result)
      }
    })
  }

  /**
   * 设置 WebRTC 开关
   */
  g.updateWebRTCStatus = (opening, callback) => {
    NimCefWebInstance.call('updateWebRTCStatus', { opening }, typeof callback === 'function' ? callback : (error, result) => {
      if (error) {
        console.log(error, result)
      }
    })
  }

  /********************************************************************************
   * 注册资料变更的回调
   */
  g.onUserInfoChanged = (callback) => {
    if (typeof callback === 'function') {
      obj.register('onUserInfoChanged', callback)
    } else {
      throw 'Invalid function'
    }
  }

  /**
   * 注册多端推送开关回调
   */
  g.onMultiportPushChanged = (callback) => {
    if (typeof callback === 'function') {
      obj.register('onMultiportPushChanged', callback)
    } else {
      throw 'Invalid function'
    }
  }

  /**
   * 注册 WebRTC 开关状态修改回调
   */
  g.onWebRTCStatusChanged = (callback) => {
    if (typeof callback === 'function') {
      obj.register('onWebRTCStatusChanged', callback)
    } else {
      throw 'Invalid function'
    }
  }
   
  obj.profile = g
})(NimCefWebInstance)