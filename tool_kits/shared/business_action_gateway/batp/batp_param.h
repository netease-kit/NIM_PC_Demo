
#ifndef _SDK_SUPPORT_PLATFORM_BUSINESS_ACTION_GATEWAY_BATP_BATP_PARAM_H_
#define _SDK_SUPPORT_PLATFORM_BUSINESS_ACTION_GATEWAY_BATP_BATP_PARAM_H_
#pragma warning( disable : 4503 ) // warning: decorated name length exceeded

//#include "sdk_support_platform/config/build_config.h"

#include <algorithm>
#include <typeinfo>

namespace nbase
{
    class BatpParam
    {
    public: // structors

        BatpParam()
            : content(0)
        {
        }

        template<typename ValueType>
        BatpParam(const ValueType& value)
            : content(new holder<ValueType>(value))
        {
        }

        BatpParam(const BatpParam& other)
            : content(other.content ? other.content->clone() : 0)
        {
        }

        ~BatpParam()
        {
            delete content;
        }

    public: // modifiers

        BatpParam& swap(BatpParam& rhs)
        {
            std::swap(content, rhs.content);
            return *this;
        }

        template<typename ValueType>
        BatpParam& operator=(const ValueType& rhs)
        {
            BatpParam(rhs).swap(*this);
            return *this;
        }

        BatpParam& operator=(BatpParam rhs)
        {
            rhs.swap(*this);
            return *this;
        }

    public: // queries

        bool empty() const
        {
            return !content;
        }

        const std::type_info& type() const
        {
            return content ? content->type() : typeid(void);
        }

#ifndef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
    private: // types
#else
    public: // types (public so BatpParamCast can be non-friend)
#endif

        class placeholder
        {
        public: // structors

            virtual ~placeholder()
            {
            }

        public: // queries

            virtual const std::type_info& type() const = 0;

            virtual placeholder* clone() const = 0;

        };

        template<typename ValueType>
        class holder : public placeholder
        {
        public: // structors

            holder(const ValueType& value)
                : held(value)
            {
            }

        public: // queries

            virtual const std::type_info& type() const
            {
                return typeid(ValueType);
            }

            virtual placeholder* clone() const
            {
                return new holder(held);
            }

        public: // representation

            ValueType held;

        private: // intentionally left unimplemented
            holder& operator=(const holder&);
        };

#ifndef BOOST_NO_MEMBER_TEMPLATE_FRIENDS

    private: // representation

        template<typename ValueType>
        friend ValueType* BatpParamCast(BatpParam*);

        template<typename ValueType>
        friend ValueType* UnsafeBatpParamCast(BatpParam*);

#else

    public: // representation (public so BatpParamCast can be non-friend)

#endif

        placeholder* content;

    };

    class BadBatpParamCast : public std::bad_cast
    {
    public:
        virtual const char* what() const throw()
        {
            return "BadBatpParamCast: "
                "failed conversion BatpParamCast";
        }
    };

    template<typename ValueType>
    ValueType* BatpParamCast(BatpParam* operand)
    {
        return operand &&
#ifdef BOOST_AUX_ANY_TYPE_ID_NAME
            std::strcmp(operand->type().name(), typeid(ValueType).name()) == 0
#else
            operand->type() == typeid(ValueType)
#endif
            ? &static_cast<BatpParam::holder<ValueType>*>(operand->content)->held
            : 0;
    }

    template<typename ValueType>
    inline const ValueType* BatpParamCast(const BatpParam* operand)
    {
        return BatpParamCast<ValueType>(const_cast<BatpParam*>(operand));
    }

    template<typename ValueType>
    ValueType BatpParamCast(BatpParam& operand)
    {
        typedef typename std::remove_reference<ValueType>::type nonref;
        static_assert(!(std::is_reference<nonref>::value), "still right reference");
        nonref* result = BatpParamCast<nonref>(&operand);
        if (!result)
        {
            throw(BadBatpParamCast());
        }

        return *result;
    }

    template<typename ValueType>
    inline ValueType BatpParamCast(const BatpParam& operand)
    {
        typedef typename std::remove_reference<ValueType>::type nonref;
        static_assert(!std::is_reference<nonref>::value, "still right reference");
        return BatpParamCast<const nonref&>(const_cast<BatpParam&>(operand));
    }
    template<typename ValueType>
    inline ValueType* UnsafeBatpParamCast(BatpParam* operand)
    {
        return &static_cast<BatpParam::holder<ValueType>*>(operand->content)->held;
    }

    template<typename ValueType>
    inline const ValueType* UnsafeBatpParamCast(const BatpParam* operand)
    {
        return UnsafeBatpParamCast<ValueType>(const_cast<BatpParam*>(operand));
    }
}

#endif//_SDK_SUPPORT_PLATFORM_BUSINESS_ACTION_GATEWAY_BATP_BATP_PARAM_H_
