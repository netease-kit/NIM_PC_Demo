((obj) => {
  const g = {}

  /**
   * 根据 chatroom id 显示一个聊天室界面
   */
  g.showChatroomById = (chatroomId) => {
    obj.call('showChatroomById', { chatroomId }, (error, result) => {
      alert(chatroomId)
      if (error) {
        console.log(result)
      }
    })
  }

  /**
   * 注册一个聊天室列表变化的回调
   */
  g.onChatroomListChanged = (callback) => {
    if (typeof callback === 'function') {
      obj.register('onChatroomListChanged', callback)
    } else {
      throw 'Invalid function'
    }
  }

  obj.chatroom = g
})(NimCefWebInstance)