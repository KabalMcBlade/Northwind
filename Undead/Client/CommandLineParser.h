#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/MemoryWrapper.h"
#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


UD_NAMESPACE_BEGIN

class UD_DLL Option
{
public:
	UD_INLINE Option() : m_isMandatory(false), m_isSet(false) {}
	UD_INLINE virtual ~Option() {}

	UD_INLINE virtual bool HasValue() const { return false; }
	UD_INLINE virtual bool NeedValue() const { return false; }
	UD_INLINE virtual bool HasDefault() const { return false; }

	UD_INLINE void SetOption(const udString& _option) { m_option = _option; }
	UD_INLINE bool IsSet() const { return m_isSet; }
	UD_INLINE bool IsMandatory() const { return m_isMandatory; }
	UD_INLINE void Set() { m_isSet = true; }
	UD_INLINE void SetMandatory(bool _isMandatory) { m_isMandatory = _isMandatory; }
	UD_INLINE const udString& GetOption() const { return m_option; }
	UD_INLINE void SetStringValue(const udString& _value)
	{
		m_stringValue = _value;
		std::transform(m_stringValue.begin(), m_stringValue.end(), m_stringValue.begin(), ::tolower);
	}

protected:
	friend class CommandLineParser;
	using OptionAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	static OptionAllocator* GetAllocator();

	udString m_option;
	udString m_stringValue;
	bool m_isMandatory;
	bool m_isSet;
};

//////////////////////////////////////////////////////////////////////////

template <class T>
class OptionValue : public Option
{
public:
	OptionValue() : Option(), m_hasDefault(false) {}
	virtual ~OptionValue() {}
	virtual bool NeedValue() const override { return true; }
	virtual bool HasValue() const override { return !m_stringValue.empty(); }
	virtual bool HasDefault() const override { return m_hasDefault; }
	void SetDefault(T _value)
	{
		m_default = _value;
		m_hasDefault = true;
	}
	const T& GetValue() const { return HasValue() ? m_value : m_default; }
	void Parse()
	{
		T var;
		eos::IStringStream<Option::OptionAllocator, Option::GetAllocator> iss;
		iss.str(m_stringValue.c_str());
		iss >> var;
		m_value = var;
	}

private:
	T m_value;
	T m_default;
	bool m_hasDefault;
};

//////////////////////////////////////////////////////////////////////////

template <>
class OptionValue<udString> : public Option
{
public:
	UD_INLINE OptionValue() : Option(), m_hasDefault(false) {}
	UD_INLINE virtual ~OptionValue() {}

	UD_INLINE virtual bool NeedValue() const override { return true; }
	UD_INLINE virtual bool HasValue() const override { return !m_stringValue.empty(); }
	UD_INLINE virtual bool HasDefault() const override { return m_hasDefault; }
	UD_INLINE void SetDefault(udString _value)
	{
		m_default = _value;
		m_hasDefault = true;
	}

	UD_INLINE const udString& GetValue() const { return HasValue() ? m_value : m_default; }
	UD_INLINE void Parse() { m_value = m_stringValue; }

private:
	udString m_value;
	udString m_default;
	bool m_hasDefault;
};

//////////////////////////////////////////////////////////////////////////

class UD_DLL CommandLineParser final
{
public:
	CommandLineParser();
	~CommandLineParser();

	void Add(const udString& _option, bool _mandatory = true);

	template <class T>
	void AddWithValue(const udString& _option, bool _mandatory = true)
	{
		OptionValue<T>* opt = ionNew(OptionValue<T>);
		opt->SetOption(_option);
		opt->SetMandatory(_mandatory);

		m_options[_option] = opt;
	}

	template <class T>
	void AddWithValueAndDefault(const udString& _option, bool _mandatory = true, const T _default = T())
	{
		OptionValue<T>* opt = eosNew(OptionValue<T>, Option::GetAllocator());
		opt->SetOption(_option);
		opt->SetMandatory(_mandatory);
		opt->SetDefault(_default);
		opt->SetStringValue(std::to_string(_default).c_str());  // redundant but easy

		m_options[_option] = opt;
	}


	bool Parse(int32 argc, const char * const argv[]);

	bool HasOption(const udString& _option);

	bool HasValue(const udString& _option);
	bool IsSet(const udString& _option);

	template <class T>
	const T GetValue(const udString& _option, const T _default = T()) const
	{
		auto search = m_options.find(_option);
		if (search != m_options.end())
		{
			OptionValue<T>* opt = dynamic_cast<OptionValue<T>*>(search->second);
			if (opt->HasValue())
			{
				opt->Parse();
				return opt->GetValue();
			}
			else
			{
				return _default;
			}
		}
		else
		{
			return _default;
		}
	}

private:
	eos::Map<udString, Option*, Option::OptionAllocator, Option::GetAllocator> m_options;
};


UD_NAMESPACE_END